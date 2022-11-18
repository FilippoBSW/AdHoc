// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#include "ScriptHandler.hpp"

#include <Input/Input.hpp>
#include <Scene/Components.hpp>
#include <Scene/Scene.hpp>
#include <Scripting/Script.hpp>

#include <Audio/Audio.hpp>

#include <Event/Event.hpp>
#include <Vulkan/Context.hpp>

namespace adh {
    int ScriptHandler::GetScene(lua_State* L) {
        lua_pushstring(L, scene->GetTag().data());
        return 1;
    }
    int ScriptHandler::GetThis(lua_State* L) {
        lua_pushinteger(L, currentEntity);
        return 1;
    }

    int ScriptHandler::DeltaTime(lua_State* L) {
        lua_pushnumber(L, deltaTime);
        return 1;
    }

    int ScriptHandler::LoadScene(lua_State* L) {
        loadSceneFilename = lua_tostring(L, 1);
        return 0;
    }

    int ScriptHandler::CreateEntity(lua_State* L) {
        auto e{ scene->GetWorld().CreateEntity() };
        auto [tag, transform, mesh, material] =
            scene->GetWorld().Add<Tag, Transform, Mesh, Material>(
                e,
                std::string("New Entity(") + std::to_string(scene->GetWorld().GetEntityCount()) + ")",
                Transform{},
                Mesh{},
                Material{});
        mesh.Load(vk::Context::Get()->GetDataDirectory() + "Assets/Models/cube.obj");
        lua_pushinteger(L, static_cast<std::uint64_t>(e));
        return 1;
    }

    int ScriptHandler::DestroyEntity(lua_State* L) {
        auto e{ static_cast<ecs::Entity>(lua_tonumber(L, 1)) };
        scene->GetWorld().Destroy(e);
        return 0;
    }

    int ScriptHandler::AddComponent(lua_State* L) {
        auto entity = static_cast<ecs::Entity>(lua_tonumber(L, 1));
        auto name   = lua_tostring(L, 2);
        if (!std::strcmp(name, "Transform")) {
            if (!scene->GetWorld().Contains<Transform>(entity)) {
                scene->GetWorld().Add<Transform>(entity, Transform{});
            }
        } else if (!std::strcmp(name, "Material")) {
            if (!scene->GetWorld().Contains<Material>(entity)) {
                scene->GetWorld().Add<Material>(entity, Material{});
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (!scene->GetWorld().Contains<Mesh>(entity)) {
                scene->GetWorld().Add<Mesh>(entity, Mesh{});
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (!scene->GetWorld().Contains<Tag>(entity)) {
                scene->GetWorld().Add<Tag>(entity, Tag{});
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (!scene->GetWorld().Contains<Camera2D>(entity)) {
                scene->GetWorld().Add<Camera2D>(entity, Camera2D{});
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (!scene->GetWorld().Contains<Camera3D>(entity)) {
                scene->GetWorld().Add<Camera3D>(entity, Camera3D{});
            }
        } else if (!std::strcmp(name, "Script")) {
            auto file = lua_tostring(L, 3);
            scriptComponentEvent.EmplaceBack([file = file, entity = entity]() {
                if (!scene->GetWorld().Contains<lua::Script>(entity)) {
                    auto [s]      = scene->GetWorld().Add<lua::Script>(entity, lua::Script{ scene->GetState().CreateScript2(file) });
                    currentEntity = static_cast<std::uint64_t>(entity);
                    s.Run();
                    s.Bind();
                    s.Call("Start");
                    s.Unbind();
                }
            });

        } else if (!std::strcmp(name, "RigidBody")) {
            if (!scene->GetWorld().Contains<RigidBody>(entity)) {
                if (lua_isstring(L, 3)) {
                    auto type = lua_tostring(L, 3);
                    PhysicsColliderShape colliderShape{ PhysicsColliderShape::eInvalid };
                    if (!std::strcmp(type, "Box")) {
                        colliderShape = PhysicsColliderShape::eBox;
                    } else if (!std::strcmp(type, "Sphere")) {
                        colliderShape = PhysicsColliderShape::eSphere;
                    } else if (!std::strcmp(type, "Capsule")) {
                        colliderShape = PhysicsColliderShape::eCapsule;
                    } else if (!std::strcmp(type, "Mesh")) {
                        colliderShape = PhysicsColliderShape::eMesh;
                    } else if (!std::strcmp(type, "ConvexMesh")) {
                        colliderShape = PhysicsColliderShape::eConvexMesh;
                    } else {
                        std::string err{ "Rigid body type not valid" };
                        Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eLog, err.data());
                        return 0;
                    }
                    if (colliderShape != PhysicsColliderShape::eInvalid &&
                        !scene->GetWorld().Contains<RigidBody>(entity) &&
                        scene->GetWorld().Contains<Transform>(entity)) {
                        auto [rigidBody]{ scene->GetWorld().Add<RigidBody>(entity, RigidBody{}) };
                        auto [transform]{ scene->GetWorld().Get<Transform>(entity) };

                        Mesh* meshPtr{ nullptr };
                        if ((colliderShape == PhysicsColliderShape::eMesh || colliderShape == PhysicsColliderShape::eConvexMesh) &&
                            scene->GetWorld().Contains<Mesh>(entity)) {
                            auto [mesh]{ scene->GetWorld().Get<Mesh>(entity) };
                            meshPtr = &mesh;
                        }
                        auto staticType = lua_tostring(L, 4);
                        bool isStatic{};
                        if (!std::strcmp(staticType, "Static")) {
                            isStatic = true;
                        } else if (!std::strcmp(staticType, "Dynamic")) {
                            isStatic = false;
                        }
                        rigidBody.Create(static_cast<std::uint64_t>(entity),
                                         0.5f,
                                         0.5f,
                                         1.0f,
                                         isStatic ? PhysicsBodyType::eStatic : PhysicsBodyType::eDynamic,
                                         1.0f,
                                         false,
                                         false,
                                         true,
                                         colliderShape,
                                         PhysicsColliderType::eCollider,
                                         transform.scale,
                                         1.0f,
                                         0.5,
                                         meshPtr);
                        physx::PxTransform t;
                        t.p = physx::PxVec3{ transform.translate.x, transform.translate.y, transform.translate.z };
                        Quaternion<float> qq(transform.rotation);
                        physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
                        t.q = q;
                        rigidBody.actor->setGlobalPose(t);
                    }
                } else {
                    std::string err{ "Need to specify the rigid body type!" };
                    Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
                }
            }
        } else {
            std::string err = "Component: [" + std::string(name) + "] is invalid!\n";
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
        }
        return 0;
    }

    int ScriptHandler::RemoveComponent(lua_State* L) {
        auto entity = static_cast<ecs::Entity>(lua_tonumber(L, 1));
        auto name   = lua_tostring(L, 2);

        if (!std::strcmp(name, "Transform")) {
            if (scene->GetWorld().Contains<Transform>(entity)) {
                scene->GetWorld().Remove<Transform>(entity);
            }
        } else if (!std::strcmp(name, "Material")) {
            if (scene->GetWorld().Contains<Material>(entity)) {
                scene->GetWorld().Remove<Material>(entity);
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (scene->GetWorld().Contains<Mesh>(entity)) {
                scene->GetWorld().Remove<Mesh>(entity);
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (scene->GetWorld().Contains<Tag>(entity)) {
                scene->GetWorld().Remove<Tag>(entity);
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (scene->GetWorld().Contains<Camera2D>(entity)) {
                scene->GetWorld().Remove<Camera2D>(entity);
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (scene->GetWorld().Contains<Camera3D>(entity)) {
                scene->GetWorld().Remove<Camera3D>(entity);
            }
        } else if (!std::strcmp(name, "RigidBody")) {
            if (scene->GetWorld().Contains<RigidBody>(entity)) {
                scene->GetWorld().Remove<RigidBody>(entity);
            }
        } else if (!std::strcmp(name, "Script")) {
            if (scene->GetWorld().Contains<lua::Script>(entity)) {
                scriptComponentEvent.EmplaceBack([scene = scene, entity = entity]() {
                    scene->GetWorld().Remove<lua::Script>(entity);
                });
            }
        } else {
            std::string err = "Component: [" + std::string(name) + "] is invalid!\n";
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
        }

        return 0;
    }

    int ScriptHandler::GetComponent(lua_State* L) {
        auto entity = static_cast<ecs::Entity>(lua_tonumber(L, 1));
        auto name   = lua_tostring(L, 2);

        if (!std::strcmp(name, "Transform")) {
            if (scene->GetWorld().Contains<Transform>(entity)) {
                auto [temp] = scene->GetWorld().Get<Transform>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Material")) {
            if (scene->GetWorld().Contains<Material>(entity)) {
                auto [temp] = scene->GetWorld().Get<Material>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (scene->GetWorld().Contains<Mesh>(entity)) {
                auto [temp] = scene->GetWorld().Get<Mesh>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Script")) {
            if (scene->GetWorld().Contains<lua::Script>(entity)) {
                auto [temp] = scene->GetWorld().Get<lua::Script>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (scene->GetWorld().Contains<Tag>(entity)) {
                auto [temp] = scene->GetWorld().Get<Tag>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (scene->GetWorld().Contains<Camera2D>(entity)) {
                auto [temp] = scene->GetWorld().Get<Camera2D>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (scene->GetWorld().Contains<Camera3D>(entity)) {
                auto [temp] = scene->GetWorld().Get<Camera3D>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "RigidBody")) {
            if (scene->GetWorld().Contains<RigidBody>(entity)) {
                auto [temp] = scene->GetWorld().Get<RigidBody>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Script")) {
            if (scene->GetWorld().Contains<lua::Script>(entity)) {
                auto [temp] = scene->GetWorld().Get<lua::Script>(entity);
                return scene->GetState().BindObject(&temp);
            }
        } else {
            std::string err = "Component: [" + std::string(name) + "] is invalid!\n";
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eLog, err.data());
        }
        return 0;
    }

    int ScriptHandler::GetInput(lua_State* L) {
        return scene->GetState().BindObject(input);
    }

    int ScriptHandler::LogMessage(lua_State* L) {
        if (lua_isstring(L, 1)) {
            std::string msg{ std::string(lua_tostring(L, 1)) + "\n" };
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eLog, msg.data());
        }
        return 0;
    }

    int ScriptHandler::LogError(lua_State* L) {
        if (lua_isstring(L, 1)) {
            std::string err{ std::string(lua_tostring(L, 1)) + "\n" };
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
        }
        return 0;
    }

    int ScriptHandler::Raycast(lua_State* L) {
        auto from      = static_cast<Vector3D**>(lua_touserdata(L, 1));
        auto direction = static_cast<Vector3D**>(lua_touserdata(L, 2));
        auto distance  = lua_tonumber(L, 3);
        auto e{ scene->GetPhysics().Raycast(**from, **direction, distance) };
        lua_pushinteger(L, e);
        return 1;
    }

    int ScriptHandler::SetGravity(lua_State* L) {
        auto x = (float)lua_tonumber(L, 1);
        auto y = (float)lua_tonumber(L, 2);
        auto z = (float)lua_tonumber(L, 3);
        scene->GetPhysics().SetGravity(Vector3D{ x, y, z });
        return 0;
    }

    // int ScriptHandler::CallScript(lua_State* L) {
    //     auto& script = **static_cast<lua::Script**>(lua_touserdata(L, 1));
    //     auto func    = lua_tostring(L, 2);
    //     script.Bind();
    //     script.Call2(func);
    //     lua_remove(L, -2);
    //     return 1;
    // }

    // TODO:
    int ScriptHandler::SerializeField(lua_State* L) {
        return 0;
    }

    void ScriptHandler::RegisterBindings() {
        auto& state = scene->GetState();

        // state.AddFunction("CallScript", ScriptHandler::CallScript);
        state.AddFunction("GetScene", ScriptHandler::GetScene);
        state.AddFunction("LoadScene", ScriptHandler::LoadScene);
        state.AddFunction("CreateEntity", ScriptHandler::CreateEntity);
        state.AddFunction("DestroyEntity", ScriptHandler::DestroyEntity);
        state.AddFunction("AddComponent", ScriptHandler::AddComponent);
        state.AddFunction("RemoveComponent", ScriptHandler::RemoveComponent);
        state.AddFunction("GetComponent", ScriptHandler::GetComponent);
        state.AddFunction("GetInput", ScriptHandler::GetInput);
        state.AddFunction("GetThis", ScriptHandler::GetThis);
        state.AddFunction("LogMessage", ScriptHandler::LogMessage);
        state.AddFunction("LogError", ScriptHandler::LogError);
        state.AddFunction("DeltaTime", ScriptHandler::DeltaTime);
        state.AddFunction("Raycast", ScriptHandler::Raycast);
        state.AddFunction("SetGravity", ScriptHandler::SetGravity);
        state.AddFunction("SerializeField", ScriptHandler::SerializeField);

        state.RegisterType<Input>("Input");
        state.RegisterTypeFunction<Input>("GetKey", &Input::RepeatGetKey);
        state.RegisterTypeFunction<Input>("GetKeyDown", &Input::GetKeyDown);
        state.RegisterTypeFunction<Input>("GetKeyUp", &Input::GetKeyUp);
        state.RegisterTypeFunction<Input>("GetButton", &Input::RepeatGetButton);
        state.RegisterTypeFunction<Input>("GetButtonDown", &Input::GetButtonDown);
        state.RegisterTypeFunction<Input>("GetButtonUp", &Input::GetButtonUp);
        state.RegisterTypeFunction<Input>("SetControllerVibration", &Input::SetControllerVibration);
        state.RegisterTypeFunction<Input>("GetMousePositionX", &Input::GetMousePositionX);
        state.RegisterTypeFunction<Input>("GetMousePositionY", &Input::GetMousePositionY);
        state.RegisterTypeFunction<Input>("GetLeftMouseButtonDown", &Input::GetLeftMouseButtonDown);
        state.RegisterTypeFunction<Input>("GetLeftMouseButtonUp", &Input::GetLeftMouseButtonUp);
        state.RegisterTypeFunction<Input>("GetRightMouseButtonDown", &Input::GetRightMouseButtonDown);
        state.RegisterTypeFunction<Input>("GetRightMouseButtonUp", &Input::GetRightMouseButtonUp);
        state.RegisterTypeFunction<Input>("GetMiddleMouseButtonDown", &Input::GetMiddleMouseButtonDown);
        state.RegisterTypeFunction<Input>("GetMiddleMouseButtonUp", &Input::GetMiddleMouseButtonUp);
        state.RegisterTypeFunction<Input>("GetMouseWheelDelta", &Input::GetMouseWheelDelta);

        state.RegisterType<Vector2D>("Vector2D");
        state.RegisterTypeConstructor<Vector2D>("new");
        state.RegisterTypeVariable<Vector2D>("x", &Vector2D::x);
        state.RegisterTypeVariable<Vector2D>("y", &Vector2D::y);

        state.RegisterType<Vector3D>("Vector3D");
        state.RegisterTypeConstructor<Vector3D>("new");
        state.RegisterTypeVariable<Vector3D>("x", &Vector3D::x);
        state.RegisterTypeVariable<Vector3D>("y", &Vector3D::y);
        state.RegisterTypeVariable<Vector3D>("z", &Vector3D::z);
        state.RegisterTypeFunction<Vector3D>("Cross", &Vector3D::Cross);

        state.RegisterType<Vector4D>("Vector4D");
        state.RegisterTypeConstructor<Vector4D>("new");
        state.RegisterTypeVariable<Vector4D>("x", &Vector4D::x);
        state.RegisterTypeVariable<Vector4D>("y", &Vector4D::y);
        state.RegisterTypeVariable<Vector4D>("z", &Vector4D::z);
        state.RegisterTypeVariable<Vector4D>("w", &Vector4D::w);

        state.RegisterType<xmm::Vector>("XmmVector");
        state.RegisterTypeConstructor<xmm::Vector>("new");
        state.RegisterTypeVariable<xmm::Vector>("x", &xmm::Vector::x);
        state.RegisterTypeVariable<xmm::Vector>("y", &xmm::Vector::y);
        state.RegisterTypeVariable<xmm::Vector>("z", &xmm::Vector::z);
        state.RegisterTypeVariable<xmm::Vector>("w", &xmm::Vector::w);

        state.RegisterType<Quaternion<float>>("Quaternion");
        state.RegisterTypeConstructor<Quaternion<float>>("new");
        state.RegisterTypeVariable<Quaternion<float>>("x", &Quaternion<float>::x);
        state.RegisterTypeVariable<Quaternion<float>>("y", &Quaternion<float>::y);
        state.RegisterTypeVariable<Quaternion<float>>("z", &Quaternion<float>::z);
        state.RegisterTypeVariable<Quaternion<float>>("w", &Quaternion<float>::w);

        state.RegisterType<Tag>("Tag");
        state.RegisterTypeConstructor<Tag>("new");
        state.RegisterTypeFunction<Tag>("Get", &Tag::Get);
        state.RegisterTypeFunction<Tag>("Set", &Tag::Set);

        state.RegisterType<Transform>("Transform");
        state.RegisterTypeConstructor<Transform>("new");
        state.RegisterTypeVariable<Transform>("translate", &Transform::translate);
        state.RegisterTypeVariable<Transform>("scale", &Transform::scale);
        state.RegisterTypeVariable<Transform>("rotation", &Transform::rotation);
        state.RegisterTypeFunction<Transform>("GetForward", &Transform::GetForward);

        state.RegisterType<Material>("Material");
        state.RegisterTypeConstructor<Material>("new");
        state.RegisterTypeVariable<Material>("roughness", &Material::roughness);
        state.RegisterTypeVariable<Material>("metallicness", &Material::metallicness);
        state.RegisterTypeVariable<Material>("transparency", &Material::transparency);
        state.RegisterTypeVariable<Material>("albedo", &Material::albedo);

        state.RegisterType<Mesh>("Mesh");
        state.RegisterTypeFunction<Mesh>("GetIndexCount", &Mesh::GetIndexCount);
        state.RegisterTypeFunction<Mesh>("Load", &Mesh::Load2);
        state.RegisterTypeVariable<Mesh>("toDraw", &Mesh::toDraw);

        state.RegisterType<Camera2D>("Camera2D");
        state.RegisterTypeConstructor<Camera2D>("new");
        state.RegisterTypeVariable<Camera2D>("eyePosition", &Camera2D::eyePosition);
        state.RegisterTypeVariable<Camera2D>("focusPosition", &Camera2D::focusPosition);
        state.RegisterTypeVariable<Camera2D>("upVector", &Camera2D::upVector);
        state.RegisterTypeVariable<Camera2D>("left", &Camera2D::left);
        state.RegisterTypeVariable<Camera2D>("right", &Camera2D::right);
        state.RegisterTypeVariable<Camera2D>("bottom", &Camera2D::bottom);
        state.RegisterTypeVariable<Camera2D>("top", &Camera2D::top);
        state.RegisterTypeVariable<Camera2D>("nearZ", &Camera2D::nearZ);
        state.RegisterTypeVariable<Camera2D>("farZ", &Camera2D::farZ);
        state.RegisterTypeVariable<Camera2D>("isSceneCamera", &Camera2D::isSceneCamera);
        state.RegisterTypeVariable<Camera2D>("isGameCamera", &Camera2D::isRuntimeCamera);

        state.RegisterType<Camera3D>("Camera3D");
        state.RegisterTypeConstructor<Camera3D>("new");
        state.RegisterTypeVariable<Camera3D>("eyePosition", &Camera3D::eyePosition);
        state.RegisterTypeVariable<Camera3D>("focusPosition", &Camera3D::focusPosition);
        state.RegisterTypeVariable<Camera3D>("upVector", &Camera3D::upVector);
        state.RegisterTypeVariable<Camera3D>("fieldOfView", &Camera3D::fieldOfView);
        state.RegisterTypeVariable<Camera3D>("nearZ", &Camera3D::nearZ);
        state.RegisterTypeVariable<Camera3D>("farZ", &Camera3D::farZ);
        state.RegisterTypeVariable<Camera3D>("isSceneCamera", &Camera3D::isSceneCamera);
        state.RegisterTypeVariable<Camera3D>("isGameCamera", &Camera3D::isRuntimeCamera);

        state.RegisterType<RigidBody>("RigidBody");
        state.RegisterTypeConstructor<RigidBody>("new");
        state.RegisterTypeVariable<RigidBody>("mass", &RigidBody::mass);
        state.RegisterTypeVariable<RigidBody>("restitution", &RigidBody::restitution);
        state.RegisterTypeVariable<RigidBody>("dynamicFriction", &RigidBody::dynamicFriction);
        state.RegisterTypeVariable<RigidBody>("staticFriction", &RigidBody::staticFriction);
        state.RegisterTypeVariable<RigidBody>("entity", &RigidBody::entity);
        state.RegisterTypeVariable<RigidBody>("isKinematic", &RigidBody::isKinematic);
        state.RegisterTypeVariable<RigidBody>("isTrigger", &RigidBody::isTrigger);
        state.RegisterTypeVariable<RigidBody>("velocity", &RigidBody::velocity);
        state.RegisterTypeVariable<RigidBody>("translate", &RigidBody::translate);
        state.RegisterTypeVariable<RigidBody>("rotation", &RigidBody::rotation);
        state.RegisterTypeVariable<RigidBody>("scale", &RigidBody::scale);
        state.RegisterTypeVariable<RigidBody>("angularVelocity", &RigidBody::angularVelocity);
        state.RegisterTypeVariable<RigidBody>("radius", &RigidBody::radius);
        state.RegisterTypeVariable<RigidBody>("halfHeight", &RigidBody::halfHeight);
        state.RegisterTypeFunction<RigidBody>("GetVelocity", &RigidBody::GetVelocity);
        state.RegisterTypeFunction<RigidBody>("GetAngularVelocity", &RigidBody::GetAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("SetLinearFactor", &RigidBody::SetLinearFactor);
        state.RegisterTypeFunction<RigidBody>("SetAngularFactor", &RigidBody::SetAngularFactor);
        state.RegisterTypeFunction<RigidBody>("SetVelocity", &RigidBody::SetVelocity);
        state.RegisterTypeFunction<RigidBody>("AddVelocity", &RigidBody::AddVelocity);
        state.RegisterTypeFunction<RigidBody>("SetAngularVelocity", &RigidBody::SetAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("AddAngularVelocity", &RigidBody::AddAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("GetIsTrigger", &RigidBody::GetIsTrigger);
        state.RegisterTypeFunction<RigidBody>("SetTrigger", &RigidBody::SetTrigger);
        state.RegisterTypeFunction<RigidBody>("SetKinematic", &RigidBody::SetKinematic);
        state.RegisterTypeFunction<RigidBody>("SetTranslation", &RigidBody::SetTranslation);
        state.RegisterTypeFunction<RigidBody>("GetTranslation", &RigidBody::GetTranslation);
        state.RegisterTypeFunction<RigidBody>("SetMass", &RigidBody::SetMass);
        state.RegisterTypeFunction<RigidBody>("GetMass", &RigidBody::GetMass);
        state.RegisterTypeFunction<RigidBody>("SetPosition", &RigidBody::SetPosition);
        state.RegisterTypeFunction<RigidBody>("SetRotation", &RigidBody::SetRotation);
        state.RegisterTypeFunction<RigidBody>("AddRotation", &RigidBody::AddRotation);
        state.RegisterTypeFunction<RigidBody>("GetRotation", &RigidBody::GetRotation);
        state.RegisterTypeFunction<RigidBody>("ClearForces", &RigidBody::ClearForces);
        state.RegisterTypeFunction<RigidBody>("AddForce", &RigidBody::AddForce);
        state.RegisterTypeFunction<RigidBody>("AddTorque", &RigidBody::AddTorque);
        state.RegisterTypeFunction<RigidBody>("GetRestitution", &RigidBody::GetRestitution);
        state.RegisterTypeFunction<RigidBody>("SetRestitution", &RigidBody::SetRestitution);
        state.RegisterTypeFunction<RigidBody>("GetStaticFriction", &RigidBody::GetStaticFriction);
        state.RegisterTypeFunction<RigidBody>("SetStaticFriction", &RigidBody::SetStaticFriction);
        state.RegisterTypeFunction<RigidBody>("GetDynamicFriction", &RigidBody::GetDynamicFriction);
        state.RegisterTypeFunction<RigidBody>("SetDynamicFriction", &RigidBody::SetDynamicFriction);
        state.RegisterTypeFunction<RigidBody>("UpdateGeometry", &RigidBody::UpdateGeometry);
        state.RegisterTypeFunction<RigidBody>("SetHasGravity", &RigidBody::SetHasGravity);

        state.RegisterType<Audio>("Audio");
        state.RegisterTypeConstructor<Audio>("new");
        // state.RegisterTypeFunction<Audio>("OnUpdate", &Audio::OnUpdate);
        state.RegisterTypeFunction<Audio>("Create", &Audio::Create2);
        state.RegisterTypeFunction<Audio>("Play", &Audio::Play);
        state.RegisterTypeFunction<Audio>("Stop", &Audio::Stop);
        state.RegisterTypeFunction<Audio>("Pause", &Audio::Pause);
        // state.RegisterTypeFunction<Audio>("Resume", &Audio::Resume);
        state.RegisterTypeFunction<Audio>("Loop", &Audio::Loop);
        state.RegisterTypeFunction<Audio>("IsPlaying", &Audio::IsPlaying);

        state.RegisterType<lua::Script>("Script");
        state.RegisterTypeFunction<lua::Script>("Call", &lua::Script::Call2);
        state.RegisterTypeFunction<lua::Script>("Get", &lua::Script::Get);
    }
} // namespace adh
