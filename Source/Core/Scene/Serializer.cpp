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

#include "Serializer.hpp"
#include "ComponentsSerializer.hpp"

#include <Scene/Scene.hpp>
#include <Scripting/Script.hpp>
#include <Scripting/ScriptHandler.hpp>

#include <Std/File.hpp>
#include <Vulkan/Context.hpp>

#include <Vulkan/Context.hpp>

namespace adh {
    template <typename T>
    static void SerializeComponent(YAML::Emitter& out, const char* name, T func) {
        out << YAML::Key << name;
        out << YAML::BeginMap;
        func();
        out << YAML::EndMap;
    }

    Serializer::Serializer(Scene* scene)
        : m_Scene{ scene } {
    }

    void Serializer::Serialize() {
        YAML::Emitter out;

        out << YAML::BeginMap << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Tag;
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        m_Scene->m_World.ForEach([&](auto entity) {
            out << YAML::BeginMap;
            out << YAML::Key << "Entity" << YAML::Value << static_cast<ecs::EntityID>(entity);

            if (m_Scene->m_World.Contains<Tag>(entity)) {
                SerializeComponent(out, "Tag", [&]() {
                    auto [tag] = m_Scene->m_World.Get<Tag>(entity);
                    out << YAML::Key << "tag" << YAML::Value << tag.Get();
                });
            }

            if (m_Scene->m_World.Contains<Transform>(entity)) {
                SerializeComponent(out, "Transform", [&]() {
                    auto [tranfrom] = m_Scene->m_World.Get<Transform>(entity);
                    out << YAML::Key << "translate" << YAML::Value << tranfrom.translate;
                    out << YAML::Key << "rotation" << YAML::Value << tranfrom.rotation;
                    out << YAML::Key << "scale" << YAML::Value << tranfrom.scale;
                });
            }

            if (m_Scene->m_World.Contains<Mesh>(entity)) {
                SerializeComponent(out, "Mesh", [&]() {
                    auto [mesh] = m_Scene->m_World.Get<Mesh>(entity);
                    auto&& meshBuffer{ mesh.Get() };
                    out << YAML::Key << "name" << YAML::Value << meshBuffer->meshName;
                    out << YAML::Key << "draw" << YAML::Value << mesh.toDraw;
                });
            }

            if (m_Scene->m_World.Contains<Material>(entity)) {
                SerializeComponent(out, "Material", [&]() {
                    auto [material] = m_Scene->m_World.Get<Material>(entity);
                    out << YAML::Key << "roughness" << YAML::Value << material.roughness;
                    out << YAML::Key << "metallicness" << YAML::Value << material.metallicness;
                    out << YAML::Key << "albedo" << YAML::Value << material.albedo;
                });
            }

            if (m_Scene->m_World.Contains<lua::Script>(entity)) {
                SerializeComponent(out, "Script", [&]() {
                    auto [script] = m_Scene->m_World.Get<lua::Script>(entity);
                    out << YAML::Key << "name" << YAML::Value << script.fileName;
                });
            }

            if (m_Scene->m_World.Contains<RigidBody>(entity)) {
                SerializeComponent(out, "RigidBody", [&]() {
                    auto [rigidbody] = m_Scene->m_World.Get<RigidBody>(entity);
                    out << YAML::Key << "mass" << YAML::Value << rigidbody.mass;
                    out << YAML::Key << "restitution" << YAML::Value << rigidbody.restitution;
                    out << YAML::Key << "static friction" << YAML::Value << rigidbody.staticFriction;
                    out << YAML::Key << "dynamic friction" << YAML::Value << rigidbody.dynamicFriction;
                    out << YAML::Key << "is trigger" << YAML::Value << rigidbody.isTrigger;
                    out << YAML::Key << "is kinematic" << YAML::Value << rigidbody.isKinematic;
                    out << YAML::Key << "scale is same as model" << YAML::Value << rigidbody.scaleSameAsModel;
                    out << YAML::Key << "collider type" << YAML::Value << (uint64_t)rigidbody.colliderType;
                    out << YAML::Key << "collider shape" << YAML::Value << (uint64_t)rigidbody.colliderShape;
                    out << YAML::Key << "body type" << YAML::Value << (uint64_t)rigidbody.bodyType;
                    out << YAML::Key << "entity" << YAML::Value << (uint64_t)rigidbody.entity;
                    out << YAML::Key << "scale" << YAML::Value << rigidbody.scale;
                    out << YAML::Key << "radius" << YAML::Value << rigidbody.radius;
                    out << YAML::Key << "half height" << YAML::Value << rigidbody.halfHeight;
                });
            }

            if (m_Scene->m_World.Contains<Camera2D>(entity)) {
                SerializeComponent(out, "Camera2D", [&]() {
                    auto [camera] = m_Scene->m_World.Get<Camera2D>(entity);
                    out << YAML::Key << "eye position" << YAML::Value << camera.eyePosition;
                    out << YAML::Key << "focus position" << YAML::Value << camera.focusPosition;
                    out << YAML::Key << "up vector" << YAML::Value << camera.upVector;
                    out << YAML::Key << "left" << YAML::Value << camera.left;
                    out << YAML::Key << "right" << YAML::Value << camera.right;
                    out << YAML::Key << "bottom" << YAML::Value << camera.bottom;
                    out << YAML::Key << "top" << YAML::Value << camera.top;
                    out << YAML::Key << "nearZ" << YAML::Value << camera.nearZ;
                    out << YAML::Key << "farZ" << YAML::Value << camera.farZ;
                    out << YAML::Key << "is runtime camera" << YAML::Value << camera.isRuntimeCamera;
                    out << YAML::Key << "is scene camera" << YAML::Value << camera.isSceneCamera;
                });
            }

            if (m_Scene->m_World.Contains<Camera3D>(entity)) {
                SerializeComponent(out, "Camera3D", [&]() {
                    auto [camera] = m_Scene->m_World.Get<Camera3D>(entity);
                    out << YAML::Key << "eye position" << YAML::Value << camera.eyePosition;
                    out << YAML::Key << "focus position" << YAML::Value << camera.focusPosition;
                    out << YAML::Key << "up vector" << YAML::Value << camera.upVector;
                    out << YAML::Key << "field of view" << YAML::Value << camera.fieldOfView;
                    out << YAML::Key << "aspect ratio" << YAML::Value << camera.aspectRatio;
                    out << YAML::Key << "nearZ" << YAML::Value << camera.nearZ;
                    out << YAML::Key << "farZ" << YAML::Value << camera.farZ;
                    out << YAML::Key << "is runtime camera" << YAML::Value << camera.isRuntimeCamera;
                    out << YAML::Key << "is scene camera" << YAML::Value << camera.isSceneCamera;
                });
            }

            out << YAML::EndMap;
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;
        m_Data = out.c_str();
    }

    void Serializer::SerializeToFile(const char* filePath) {
        Serialize();
        File file(filePath, File::Mode::Write);
        file.Write(m_Data.c_str(), m_Data.size());
    }

    void Serializer::Deserialize() {
        auto node{ YAML::Load(m_Data.data()) };
        Deserialize(&node);
    }

    void Serializer::DeserializeFromFile(const char* filePath) {
        YAML::Node node{ YAML::LoadFile(filePath) };
        Deserialize(&node);
    }

    void Serializer::Deserialize(void* pNode) {
        YAML::Node& node = *(YAML::Node*)(pNode);
        auto scene       = node["Scene"];
        m_Scene->m_Tag   = scene.as<std::string>();
        m_Scene->m_World.Reset();
        m_Scene->GetPhysics().ResetScene();
        m_Scene->m_State     = lua::NewState();
        ScriptHandler::scene = m_Scene;
        ScriptHandler::RegisterBindings();

        auto& world   = m_Scene->m_World;
        auto entities = node["Entities"];
        if (entities) {
            for (auto&& i : entities) {
                ecs::Entity e{ world.CreateEntity() };

                auto tag = i["Tag"];
                if (tag) {
                    world.Add<Tag>(e, std::string(tag["tag"].as<std::string>()));
                }

                auto transform = i["Transform"];
                if (transform) {
                    auto [t] = world.Add<Transform>(e,
                                                    Transform(transform["translate"].as<Vector3D>(),
                                                              transform["rotation"].as<Vector3D>(),
                                                              transform["scale"].as<Vector3D>()));
                }

                auto mesh = i["Mesh"];
                if (mesh) {
                    auto [m]             = world.Add<Mesh>(e, Mesh{});
                    std::string filePath = vk::Context::Get()->GetDataDirectory() + "Assets/Models/" + mesh["name"].as<std::string>();
                    m.Load(filePath);
                    m.toDraw = mesh["draw"].as<bool>();
                }

                auto material = i["Material"];
                if (material) {
                    world.Add<Material>(e,
                                        Material(material["roughness"].as<float>(),
                                                 material["metallicness"].as<float>(),
                                                 material["albedo"].as<Vector3D>()));
                }

                auto script = i["Script"];
                if (script) {
                    std::string filePath = vk::Context::Get()->GetDataDirectory() + "Assets/Scripts/" + script["name"].as<std::string>();
                    world.Add<lua::Script>(e, m_Scene->m_State.CreateScript(filePath));
                }

                auto rigidbody = i["RigidBody"];
                if (rigidbody && transform) {
                    auto [r]{ m_Scene->GetWorld().Add<RigidBody>(e, RigidBody{}) };
                    auto [t]{ m_Scene->GetWorld().Get<Transform>(e) };

                    Mesh* p{ nullptr };
                    if (mesh) {
                        auto [m]{ m_Scene->GetWorld().Get<Mesh>(e) };
                        p = &m;
                    }
                    r.Create(static_cast<std::uint64_t>(e),
                             rigidbody["static friction"].as<float>(),
                             rigidbody["dynamic friction"].as<float>(),
                             rigidbody["restitution"].as<float>(),
                             PhysicsBodyType(rigidbody["body type"].as<int>()),
                             rigidbody["mass"].as<float>(),
                             rigidbody["is kinematic"].as<bool>(),
                             rigidbody["is trigger"].as<bool>(),
                             rigidbody["scale is same as model"].as<bool>(),
                             PhysicsColliderShape(rigidbody["collider shape"].as<int>()),
                             PhysicsColliderType(rigidbody["collider type"].as<int>()),
                             rigidbody["scale"].as<Vector3D>(),
                             rigidbody["radius"].as<float>(),
                             rigidbody["half height"].as<float>(),
                             p);
                }

                auto camera2D = i["Camera2D"];
                if (camera2D) {
                    auto [c]          = world.Add<Camera2D>(e, Camera2D{});
                    c.eyePosition     = camera2D["eye position"].as<Vector3D>();
                    c.focusPosition   = camera2D["focus position"].as<Vector3D>();
                    c.upVector        = camera2D["up vector"].as<Vector3D>();
                    c.left            = camera2D["left"].as<float>();
                    c.right           = camera2D["right"].as<float>();
                    c.bottom          = camera2D["bottom"].as<float>();
                    c.top             = camera2D["top"].as<float>();
                    c.nearZ           = camera2D["nearZ"].as<float>();
                    c.farZ            = camera2D["farZ"].as<float>();
                    c.isSceneCamera   = camera2D["is scene camera"].as<bool>();
                    c.isRuntimeCamera = camera2D["is runtime camera"].as<bool>();
                }

                auto camera3D = i["Camera3D"];
                if (camera3D) {
                    auto [c]          = world.Add<Camera3D>(e, Camera3D{});
                    c.eyePosition     = camera3D["eye position"].as<Vector3D>();
                    c.focusPosition   = camera3D["focus position"].as<Vector3D>();
                    c.upVector        = camera3D["up vector"].as<Vector3D>();
                    c.fieldOfView     = camera3D["field of view"].as<float>();
                    c.aspectRatio     = camera3D["aspect ratio"].as<float>();
                    c.nearZ           = camera3D["nearZ"].as<float>();
                    c.farZ            = camera3D["farZ"].as<float>();
                    c.isSceneCamera   = camera3D["is scene camera"].as<bool>();
                    c.isRuntimeCamera = camera3D["is runtime camera"].as<bool>();
                }
            }
        }
    }
} // namespace adh
