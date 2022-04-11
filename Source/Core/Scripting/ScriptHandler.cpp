// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
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

#include <Event/Event.hpp>
#include <Vulkan/Context.hpp>

namespace adh {
    int ScriptHandler::GetThis(lua_State* L) {
        lua_pushinteger(L, currentEntity);
        return 1;
    }

    int ScriptHandler::DeltaTime(lua_State* L) {
        lua_pushnumber(L, deltaTime);
        return 1;
    }

    // int ScriptHandler::DispatchEvent(lua_State* L) {
    //     return 0;
    // }

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
        auto e{ ecs::Entity(lua_tonumber(L, 1)) };
        scene->GetWorld().Destroy(e);
        return 0;
    }

    int ScriptHandler::AddComponent(lua_State* L) {
        auto entity = static_cast<ecs::Entity>(lua_tonumber(L, 1));
        auto name   = lua_tostring(L, 2);
        if (!std::strcmp(name, "Transform")) {
            if (!scene->GetWorld().Contains<Transform>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Transform>(entity, Transform{});
            }
        } else if (!std::strcmp(name, "Material")) {
            if (!scene->GetWorld().Contains<Material>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Material>(entity, Material{});
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (!scene->GetWorld().Contains<Mesh>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Mesh>(entity, Mesh{});
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (!scene->GetWorld().Contains<Tag>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Tag>(entity, Tag{});
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (!scene->GetWorld().Contains<Camera2D>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Camera2D>(entity, Camera2D{});
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (!scene->GetWorld().Contains<Camera3D>(ecs::Entity(entity))) {
                scene->GetWorld().Add<Camera3D>(entity, Camera3D{});
            }
        } else if (!std::strcmp(name, "RigidBody")) {
            if (!scene->GetWorld().Contains<RigidBody>(ecs::Entity(entity))) {
                // if (lua_isstring(L, 3)) {
                //     auto type = lua_tostring(L, 3);
                //     RigidBody::ColliderShape colliderType{ RigidBody::ColliderShape::eInvalid };
                //     if (!std::strcmp(type, "Box")) {
                //         colliderType = RigidBody::ColliderShape::eBox;
                //     } else if (!std::strcmp(type, "Sphere")) {
                //         colliderType = RigidBody::ColliderShape::eSphere;
                //     } else if (!std::strcmp(type, "Cone")) {
                //         colliderType = RigidBody::ColliderShape::eCone;
                //     } else if (!std::strcmp(type, "Cylinder")) {
                //         colliderType = RigidBody::ColliderShape::eCylinder;
                //     } else if (!std::strcmp(type, "Capsule")) {
                //         colliderType = RigidBody::ColliderShape::eCapsule;
                //     } else if (!std::strcmp(type, "Mesh")) {
                //         colliderType = RigidBody::ColliderShape::eMesh;
                //     } else {
                //         std::string err{ "Rigid body type not valid" };
                //         Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eLog, err.data());
                //         return 0;
                //     }
                //     if (colliderType != RigidBody::ColliderShape::eInvalid &&
                //         !scene->GetWorld().Contains<RigidBody>(entity) &&
                //         scene->GetWorld().Contains<Transform>(entity)) {
                //         auto [rigidBody]{ scene->GetWorld().Add<RigidBody>(entity, RigidBody{}) };
                //         auto [transform]{ scene->GetWorld().Get<Transform>(entity) };

                //         if (colliderType == RigidBody::ColliderShape::eMesh &&
                //             scene->GetWorld().Contains<Mesh>(entity)) {
                //             auto [mesh]{ scene->GetWorld().Get<Mesh>(entity) };
                //             // rigidBody.Create(scene->GetPhysics().m_World, colliderType, (std::uint64_t)entity,
                //             //                  transform, 1.0f, 0.0f, 0.5f, mesh.vertex);
                //             rigidBody.Create(&scene->GetPhysics(), colliderType, (std::uint64_t)entity,
                //                              transform, 1.0f, 0.0f, 0.5f, mesh.vertex);
                //         } else {
                //             // rigidBody.Create(scene->GetPhysics().m_World, colliderType, (std::uint64_t)entity,
                //             //                  transform, 1.0f, 0.0f, 0.5f);
                //             rigidBody.Create(&scene->GetPhysics(), colliderType, (std::uint64_t)entity,
                //                              transform, 1.0f, 0.0f, 0.5f);
                //         }
                //     }
                // } else {
                //     std::string err{ "Need to specify the rigid body type!" };
                //     Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
                // }
            }
        } else {
            std::string err = "Component: [" + std::string(name) + "] is invalid!\n";
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
        }
        return 0;
    }

    int ScriptHandler::RemoveComponent(lua_State* L) {
        auto entity = lua_tonumber(L, 1);
        auto name   = lua_tostring(L, 2);

        if (!std::strcmp(name, "Transform")) {
            if (scene->GetWorld().Contains<Transform>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Transform>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "Material")) {
            if (scene->GetWorld().Contains<Material>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Material>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (scene->GetWorld().Contains<Mesh>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Mesh>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (scene->GetWorld().Contains<Tag>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Tag>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (scene->GetWorld().Contains<Camera2D>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Camera2D>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (scene->GetWorld().Contains<Camera3D>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<Camera3D>(ecs::Entity(entity));
            }
        } else if (!std::strcmp(name, "RigidBody")) {
            if (scene->GetWorld().Contains<RigidBody>(ecs::Entity(entity))) {
                scene->GetWorld().Remove<RigidBody>(ecs::Entity(entity));
            }
        } else {
            std::string err = "Component: [" + std::string(name) + "] is invalid!\n";
            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, err.data());
        }

        return 0;
    }

    int ScriptHandler::GetComponent(lua_State* L) {
        auto entity = lua_tonumber(L, 1);
        auto name   = lua_tostring(L, 2);

        if (!std::strcmp(name, "Transform")) {
            if (scene->GetWorld().Contains<Transform>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Transform>(ecs::Entity(entity));
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Material")) {
            if (scene->GetWorld().Contains<Material>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Material>(ecs::Entity(entity));
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Mesh")) {
            if (scene->GetWorld().Contains<Mesh>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Mesh>(ecs::Entity(entity));
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Tag")) {
            if (scene->GetWorld().Contains<Tag>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Tag>(ecs::Entity(entity));
                lua_pushstring(L, temp.Get());
                return 1;
            }
        } else if (!std::strcmp(name, "Camera2D")) {
            if (scene->GetWorld().Contains<Camera2D>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Camera2D>(ecs::Entity(entity));
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "Camera3D")) {
            if (scene->GetWorld().Contains<Camera3D>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<Camera3D>(ecs::Entity(entity));
                return scene->GetState().BindObject(&temp);
            }
        } else if (!std::strcmp(name, "RigidBody")) {
            if (scene->GetWorld().Contains<RigidBody>(ecs::Entity(entity))) {
                auto [temp] = scene->GetWorld().Get<RigidBody>(ecs::Entity(entity));
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
        lua_pushinteger(L, static_cast<RigidBody*>(e)->entity);
        return 1;
    }

    // int ScriptHandler::RaycastAll(lua_State* L) {
    //     auto from      = static_cast<Vector3D**>(lua_touserdata(L, 1));
    //     auto direction = static_cast<Vector3D**>(lua_touserdata(L, 2));
    //     auto distance  = lua_tonumber(L, 3);
    //     auto e{ scene->GetPhysics().RaycastAll(**from, **direction, distance) };
    //     std::cout << e.GetSize() << std::endl;
    //     if (!e.IsEmpty()) {
    //         for (auto&& i : e) {
    //             // std::cout << i << std::endl;
    //             lua_pushinteger(L, i);
    //         }
    //     }
    //     return 1;
    // }

    void ScriptHandler::RegisterBindings() {
        auto& state = scene->GetState();

        lua_pushstring(state, "CreateEntity");
        lua_pushcclosure(state, ScriptHandler::CreateEntity, 1);
        lua_setglobal(state, "CreateEntity");

        lua_pushstring(state, "DestroyEntity");
        lua_pushcclosure(state, ScriptHandler::DestroyEntity, 1);
        lua_setglobal(state, "DestroyEntity");

        lua_pushstring(state, "AddComponent");
        lua_pushcclosure(state, ScriptHandler::AddComponent, 1);
        lua_setglobal(state, "AddComponent");

        lua_pushstring(state, "RemoveComponent");
        lua_pushcclosure(state, ScriptHandler::RemoveComponent, 1);
        lua_setglobal(state, "RemoveComponent");

        lua_pushstring(state, "GetComponent");
        lua_pushcclosure(state, ScriptHandler::GetComponent, 1);
        lua_setglobal(state, "GetComponent");

        lua_pushstring(state, "GetInput");
        lua_pushcclosure(state, ScriptHandler::GetInput, 1);
        lua_setglobal(state, "GetInput");

        lua_pushstring(state, "GetThis");
        lua_pushcclosure(state, ScriptHandler::GetThis, 1);
        lua_setglobal(state, "GetThis");

        lua_pushstring(state, "LogMessage");
        lua_pushcclosure(state, ScriptHandler::LogMessage, 1);
        lua_setglobal(state, "LogMessage");

        lua_pushstring(state, "LogError");
        lua_pushcclosure(state, ScriptHandler::LogError, 1);
        lua_setglobal(state, "LogError");

        lua_pushstring(state, "DeltaTime");
        lua_pushcclosure(state, ScriptHandler::DeltaTime, 1);
        lua_setglobal(state, "DeltaTime");

        lua_pushstring(state, "Raycast");
        lua_pushcclosure(state, ScriptHandler::Raycast, 1);
        lua_setglobal(state, "Raycast");

        // lua_pushstring(state, "RaycastAll");
        // lua_pushcclosure(state, ScriptHandler::RaycastAll, 1);
        // lua_setglobal(state, "RaycastAll");

        // lua_pushstring(state, "DispatchEvent");
        // lua_pushcclosure(state, ScriptHandler::DispatchEvent, 1);
        // lua_setglobal(state, "DispatchEvent");

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
        state.RegisterTypeVariable<RigidBody>("velocity", &RigidBody::velocity);
        state.RegisterTypeFunction<RigidBody>("GetVelocity", &RigidBody::GetVelocity);
        state.RegisterTypeVariable<RigidBody>("angularVelocity", &RigidBody::angularVelocity);
        state.RegisterTypeFunction<RigidBody>("GetAngularVelocity", &RigidBody::GetAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("SetLinearFactor", &RigidBody::SetLinearFactor);
        state.RegisterTypeFunction<RigidBody>("SetAngularFactor", &RigidBody::SetAngularFactor);
        state.RegisterTypeFunction<RigidBody>("SetVelocity", &RigidBody::SetVelocity);
        state.RegisterTypeFunction<RigidBody>("AddVelocity", &RigidBody::AddVelocity);
        state.RegisterTypeFunction<RigidBody>("SetAngularVelocity", &RigidBody::SetAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("AddAngularVelocity", &RigidBody::AddAngularVelocity);
        state.RegisterTypeFunction<RigidBody>("GetIsTrigger", &RigidBody::GetIsTrigger);
        state.RegisterTypeFunction<RigidBody>("SetTrigger", &RigidBody::SetTrigger);

        state.RegisterTypeFunction<RigidBody>("SetTranslation", &RigidBody::SetTranslation);
        state.RegisterTypeFunction<RigidBody>("GetTranslation", &RigidBody::GetTranslation);
        state.RegisterTypeVariable<RigidBody>("translate", &RigidBody::translate);

        state.RegisterTypeFunction<RigidBody>("SetRotation", &RigidBody::SetRotation);
        state.RegisterTypeFunction<RigidBody>("AddRotation", &RigidBody::AddRotation);
        state.RegisterTypeFunction<RigidBody>("GetRotation", &RigidBody::GetRotation);
        state.RegisterTypeVariable<RigidBody>("rotation", &RigidBody::rotation);

        state.RegisterTypeFunction<RigidBody>("ClearForces", &RigidBody::ClearForces);

        state.RegisterTypeFunction<RigidBody>("AddForce", &RigidBody::AddForce);
        state.RegisterTypeFunction<RigidBody>("AddTorque", &RigidBody::AddTorque);
    }
} // namespace adh