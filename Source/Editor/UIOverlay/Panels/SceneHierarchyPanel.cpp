// *********************************************************************************
// MIT License
//
// Copyright (c) 2022 Filippo-BSW
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

#include "SceneHierarchyPanel.hpp"
#include <ImGui/imgui.h>
#include <Scene/Components.hpp>
#include <Vulkan/Context.hpp>

namespace ui = ImGui;

namespace adh {
    void SceneHierarchyPanel::Draw(Scene* scene, ecs::Entity& entity) noexcept {
        selectedEntity = &entity;
        currentScene   = scene;

        if (isOpen) {
            if (!ui::Begin("Scene Hierarchy", &isOpen)) {
                ui::End();
            } else {
                scene->GetWorld().GetSystem<Tag>().ForEach([&](ecs::Entity e, Tag& tag) {
                    DrawEntities(e, tag);
                });

                if (ui::IsMouseDown(0) && ui::IsWindowHovered()) {
                    *selectedEntity = ecs::null_entity;
                }

                if (ui::BeginPopupContextWindow(nullptr, 1, false)) {
                    if (ui::MenuItem("Create New Entity")) {
                        ecs::Entity e{ scene->GetWorld().CreateEntity() };
                        auto [tag, transform, mesh, material] =
                            scene->GetWorld().Add<Tag, Transform, Mesh, Material>(
                                e,
                                std::string("New Entity(") + std::to_string(scene->GetWorld().GetEntityCount()) + ")",
                                Transform{},
                                Mesh{},
                                Material{});
                        mesh.Load(vk::Context::Get()->GetDataDirectory() + "Assets/Models/cube.obj");
                    }
                    ui::EndPopup();
                }
                ui::End();
            }
        }
    }

    void SceneHierarchyPanel::DrawEntities(ecs::Entity& entity, Tag& tag) noexcept {
        ImGuiTreeNodeFlags flags{ ((*selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Bullet };

        bool opened{ ui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, tag.Get()) };
        bool entityDeleted{};
        bool copyEntity{};
        static int inc;

        if (ui::IsItemClicked()) {
            *selectedEntity = entity;
        }

        if (ui::BeginPopupContextItem(tag.Get())) {
            if (ui::MenuItem("Delete Entity")) {
                entityDeleted = true;
            }
            if (ui::MenuItem("Copy Entity")) {
                copyEntity = true;
            }

            ui::EndPopup();
        }

        if (opened) {
            ui::TreePop();
        }

        if (entityDeleted) {
            currentScene->GetWorld().Destroy(entity);
        } else if (copyEntity) {
            auto& world = currentScene->GetWorld();
            auto e      = world.CreateEntity();

            if (world.Contains<Tag>(entity)) {
                auto [tag] = world.Get<Tag>(entity);
                world.Add<Tag>(e, tag.tag + "(duplicate " + std::to_string(inc++) + ")");
            }
            if (world.Contains<Transform>(entity)) {
                auto [transform] = world.Get<Transform>(entity);
                world.Add<Transform>(e, transform);
            }
            if (world.Contains<Material>(entity)) {
                auto [material] = world.Get<Material>(entity);
                world.Add<Material>(e, material);
            }
            if (world.Contains<Camera2D>(entity)) {
                auto [camera] = world.Get<Camera2D>(entity);
                world.Add<Camera2D>(e, camera);
            }
            if (world.Contains<Camera3D>(entity)) {
                auto [camera] = world.Get<Camera3D>(entity);
                world.Add<Camera3D>(e, camera);
            }
            if (world.Contains<Mesh>(entity)) {
                auto [m]             = world.Add<Mesh>(e, Mesh{});
                auto [mesh]          = world.Get<Mesh>(entity);
                std::string filePath = vk::Context::Get()->GetDataDirectory() + "Assets/Models/" + mesh.Get()->meshName;
                m.Load(filePath);
                m.toDraw = mesh.toDraw;
            }
            if (world.Contains<RigidBody>(entity) && world.Contains<Transform>(entity)) {
                auto [r]{ world.Add<RigidBody>(e, RigidBody{}) };
                auto [rb] = world.Get<RigidBody>(entity);
                auto [t]{ world.Get<Transform>(e) };

                Mesh* p{ nullptr };
                if (world.Contains<Mesh>(entity)) {
                    auto [m]{ world.Get<Mesh>(entity) };
                    p = &m;
                }
                r.Create(static_cast<std::uint64_t>(e),
                         rb.staticFriction,
                         rb.dynamicFriction,
                         rb.restitution,
                         rb.bodyType,
                         rb.mass,
                         rb.isKinematic,
                         rb.isTrigger,
                         rb.scaleSameAsModel,
                         rb.colliderShape,
                         rb.colliderType,
                         rb.scale,
                         rb.radius,
                         rb.halfHeight,
                         p);
            }
            if (world.Contains<lua::Script>(entity)) {
                auto [script]        = world.Get<lua::Script>(entity);
                std::string filePath = vk::Context::Get()->GetDataDirectory() + "Assets/Scripts/" + script.fileName;
                world.Add<lua::Script>(e, currentScene->GetState().CreateScript(filePath));
            }
            if (world.Contains<vk::Texture2D>(entity)) {
                auto [texture2d]     = world.Get<vk::Texture2D>(entity);
                std::string filePath = vk::Context::Get()->GetDataDirectory() + "Assets/Textures/" + texture2d.mFileName;
                auto [t]             = world.Add<vk::Texture2D>(e, vk::Texture2D{});
                t.Create(filePath.data(), VK_IMAGE_USAGE_SAMPLED_BIT, VK_FILTER_LINEAR);
            }
        }
    }
} // namespace adh
