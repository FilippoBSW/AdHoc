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

#include "InspectorPanel.hpp"
#include "../IconFontCppHeaders/IconFontAwesome5.hpp"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <Math/Math.hpp>
#include <Scene/Components.hpp>
#include <Vulkan/Context.hpp>

#include <Event/Event.hpp>
#include <Scripting/Script.hpp>

namespace adh {
    template <typename T, typename F>
    static void DrawComponent(const std::string& name, ecs::Entity entity, F toCall, Scene* currentScene,
                              bool removable = true, bool resettable = true) {
        const ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

        if (currentScene->GetWorld().Contains<T>(entity)) {
            auto [component]              = currentScene->GetWorld().Get<T>(entity);
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.data());
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button(ICON_FA_ALIGN_JUSTIFY, ImVec2{ lineHeight, lineHeight })) {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings")) {
                if (resettable) {
                    if (ImGui::MenuItem("Reset component")) {
                        component = std::remove_reference_t<decltype(component)>{};
                    }
                }

                if (removable) {
                    if (ImGui::MenuItem("Remove component")) {
                        removeComponent = true;
                    }
                }

                ImGui::EndPopup();
            }

            if (open) {
                toCall(component);
                ImGui::TreePop();
            }

            if (removeComponent) {
                currentScene->GetWorld().Remove<T>(entity);
            }

            ImGui::PopStyleVar();
        }
    }

    static void DrawTransform(const std::string& label, Vector3D& value, float resetValue = 0.0f, float columnWidth = 100.0f) {
        auto& io{ ImGui::GetIO() };
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight{ GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f };
        ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize)) {
            value.x = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::InvisibleButton("##padded-text", ImVec2(5, 1));
        ImGui::SameLine();
        ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });

        ImGui::SameLine();
        ImGui::InvisibleButton("##padded-text", ImVec2(10, 1));
        ImGui::SameLine();

        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize)) {
            value.y = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::InvisibleButton("##padded-text", ImVec2(5, 1));
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });

        ImGui::SameLine();
        ImGui::InvisibleButton("##padded-text", ImVec2(5, 1));
        ImGui::SameLine();

        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize)) {
            value.z = resetValue;
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::InvisibleButton("##padded-text", ImVec2(5, 1));
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }
} // namespace adh

namespace adh {
    void InspectorPanel::Draw(Scene* scene, ecs::Entity& entity) {
        selectedEntity = &entity;
        currentScene   = scene;

        if (isOpen) {
            if (!ImGui::Begin("Inspector", &isOpen)) {
                ImGui::End();
            } else {
                if (scene->GetWorld().IsValid(entity)) {
                    DrawComponents(entity);
                }

                ImGui::End();
            }
        }
    }

    void InspectorPanel::DrawComponents(ecs::Entity& entity) {
        if (currentScene->GetWorld().Contains<Tag>(entity)) {
            auto [tagComp] = currentScene->GetWorld().Get<Tag>(entity);
            auto& tag      = tagComp.tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            std::strncpy(buffer, tag.c_str(), sizeof(buffer));
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
                tag = std::string(buffer);
            }
        }

        AddComponent(entity);

        DrawComponent<Transform>(
            "Transform", entity, [](auto& component) {
                DrawTransform("Translate", component.translate); // #TODO Negative scale
                DrawTransform("Scale", component.scale, 1.0f);

                Vector3D rotation{ ToDegrees(component.rotation[0]), ToDegrees(component.rotation[1]), ToDegrees(component.rotation[2]) };
                DrawTransform("Rotation", rotation);
                component.rotation[0] = ToRadians(rotation[0]);
                component.rotation[1] = ToRadians(rotation[1]);
                component.rotation[2] = ToRadians(rotation[2]);
            },
            currentScene);

        DrawComponent<Mesh>(
            "Mesh", entity, [](auto& component) {
                std::string buffer;
                buffer.resize(256);
                auto&& meshBuffer{ component.Get() };
                if (!meshBuffer->meshName.empty()) {
                    auto pos = meshBuffer->meshName.find_last_of('/');
                    buffer   = meshBuffer->meshName.substr(pos + 1, meshBuffer->meshName.size());
                } else {
                    buffer = "Empty";
                }

                ImGui::Checkbox("Draw", &component.toDraw);
                ImGui::SameLine();

                namespace ui = ImGui;
                ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet };
                bool opened{ ui::TreeNodeEx(reinterpret_cast<void*>(0), flags, buffer.data()) };
                bool open{};

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    open = true;
                }

                if (ui::BeginPopupContextItem(buffer.data())) {
                    if (buffer != "Empty") {
                        if (ui::MenuItem("Open")) {
                            open = true;
                        }

                        ui::EndPopup();
                    }
                }

                if (open) {
#if defined(ADH_APPLE)
                    std::string cmd2 = "open " + meshBuffer->meshFilePath;
                    system(cmd2.data());
#elif defined(ADH_WINDOWS)
                    std::string cmd2 = "start " + meshBuffer->meshFilePath;

                    for (int i{}; i != cmd2.size(); ++i) {
                        if (cmd2[i] == '/') {
                            cmd2[i] = '\\';
                        }
                    }
                    system(cmd2.data());
#endif
                }

                if (opened) {
                    ui::TreePop();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                        buffer = (const char*)payload->Data;
#if defined(ADH_WINDOWS)
                        for (std::size_t i{}; i != buffer.size(); ++i) {
                            if (buffer[i] == '\\') {
                                buffer[i] = '/';
                            }
                        }
#endif
                        if (meshBuffer->meshFilePath != buffer) {
                            auto pos{ buffer.find_last_of('.') };

                            if (pos != std::string::npos) {
                                component.Load(buffer);
                            }
                        }
                    }

                    ImGui::EndDragDropTarget();
                }
            },
            currentScene, true, false);

        DrawComponent<RigidBody>(
            "RigidBody", entity, [](auto& component) {
                if (component.bodyType == PhysicsBodyType::eDynamic) {
                    ImGui::DragFloat("Mass", &component.mass, 0.001f, 0.0f, 1'000.0f);
                    ImGui::DragFloat("Static Friction", &component.staticFriction, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat("Dynamic Friction", &component.dynamicFriction, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat("Restitution", &component.restitution, 0.001f, 0.0f, 1.0f);
                }

                if (component.colliderShape == PhysicsColliderShape::eBox) {
                    DrawTransform("Scale", component.scale);
                    if (ImGui::Checkbox("Scale is same as model", &component.scaleSameAsModel)) {
                    }
                } else if (component.colliderShape == PhysicsColliderShape::eSphere) {
                    ImGui::DragFloat("Radius", &component.radius, 0.001f, 0.0f, 1'000.0f);
                } else if (component.colliderShape == PhysicsColliderShape::eCapsule) {
                    ImGui::DragFloat("Radius", &component.radius, 0.001f, 0.0f, 1'000.0f);
                    ImGui::DragFloat("Half Height", &component.halfHeight, 0.001f, 0.0f, 1'000.0f);
                }

                if (component.bodyType == PhysicsBodyType::eDynamic) {
                    if (ImGui::Checkbox("Is kinematic", &component.isKinematic)) {
                    }
                    ImGui::SameLine();
                }
                if (ImGui::Checkbox("Is trigger", &component.isTrigger)) {
                }
            },
            currentScene, true, false);

        DrawComponent<Material>(
            "Material", entity, [](auto& component) {
                ImGui::ColorEdit3("Albedo", (float*)component.albedo.v);
                ImGui::DragFloat("Roughness", &component.roughness, 0.001f, 0.0f, 1.0f);
                ImGui::DragFloat("Metallicness", &component.metallicness, 0.001f, 0.0f, 1.0f);
            },
            currentScene);

        DrawComponent<Camera2D>(
            "Camera2D", entity, [](auto& component) {
                DrawTransform("Eye Position", component.eyePosition);
                DrawTransform("Focus Position", component.focusPosition);
                DrawTransform("Up Vector", component.upVector);

                ImGui::DragFloat("Left", &component.left, 0.001f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Right", &component.right, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Bottom", &component.bottom, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Top", &component.top, 0.01f, -1000.0f, 1000.0f);
                ImGui::DragFloat("Near Z", &component.nearZ, 0.01f, 0.1f, 1000.0f);
                ImGui::DragFloat("Far Z", &component.farZ, 0.01f, 10.0f, 1000.0f);

                ImGui::Checkbox("Scene camera", &component.isSceneCamera);
                ImGui::Checkbox("Runtime camera", &component.isRuntimeCamera);
            },
            currentScene);

        DrawComponent<Camera3D>(
            "Camera3D", entity, [](auto& component) {
                DrawTransform("Eye Position", component.eyePosition);
                DrawTransform("Focus Position", component.focusPosition);
                DrawTransform("Up Vector", component.upVector);

                ImGui::DragFloat("Field of View", &component.fieldOfView, 0.0f, 0.0f, 200.0f);
                ImGui::DragFloat("Aspect Ratio", &component.aspectRatio, 0.01f, 1.0f, 100.0f);
                ImGui::DragFloat("Near Z", &component.nearZ, 0.0f, 0.1f, 1000.0f);
                ImGui::DragFloat("Far Z", &component.farZ, 0.0f, 10.0f, 1000.0f);

                ImGui::Checkbox("Scene camera", &component.isSceneCamera);
                ImGui::Checkbox("Runtime camera", &component.isRuntimeCamera);
            },
            currentScene);

        DrawComponent<lua::Script>(
            "Script", entity, [currentScene = currentScene](auto& component) {
                std::string buffer;
                buffer.resize(256);

                if (!component.filePath.empty()) {
                    auto pos = component.filePath.find_last_of('/');
                    buffer   = component.filePath.substr(pos + 1, component.filePath.size());
                } else {
                    buffer = "Empty";
                }

                namespace ui = ImGui;
                ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Bullet };
                bool opened{ ui::TreeNodeEx(reinterpret_cast<void*>(0), flags, buffer.data()) };
                bool open{};

                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    open = true;
                }

                if (ui::BeginPopupContextItem(buffer.data())) {
                    if (ui::MenuItem("Open")) {
                        open = true;
                    }

                    ui::EndPopup();
                }

                if (open) {
#if defined(ADH_APPLE)
                    std::string cmd2 = "open " + component.filePath;
                    system(cmd2.data());
#elif defined(ADH_WINDOWS)
                    std::string cmd2 = "start " + component.filePath;

                    for (int i{}; i != cmd2.size(); ++i) {
                        if (cmd2[i] == '/') {
                            cmd2[i] = '\\';
                        }
                    }
                    system(cmd2.data());
#endif
                }

                if (opened) {
                    ui::TreePop();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                        buffer = (const char*)payload->Data;
#if defined(ADH_WINDOWS)
                        for (std::size_t i{}; i != buffer.size(); ++i) {
                            if (buffer[i] == '\\') {
                                buffer[i] = '/';
                            }
                        }
#endif

                        if (component.filePath != buffer) {
                            auto pos{ buffer.find_last_of('.') };
                            auto type{ buffer.substr(pos + 1) };
                            if (type == "lua") {
                                component = currentScene->GetState().CreateScript(buffer);
                            }
                        }
                    }

                    ImGui::EndDragDropTarget();
                }
            },
            currentScene, true, false);
    }

    void InspectorPanel::AddComponent(ecs::Entity& entity) {
        static bool scriptInputName;

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponent");
        }

        if (ImGui::BeginPopup("AddComponent")) {
            if (ImGui::MenuItem("Tag")) {
                if (!currentScene->GetWorld().Contains<Tag>(entity)) {
                    currentScene->GetWorld().Add<Tag>(entity, Tag{});
                }
            }

            if (ImGui::MenuItem("Transform")) {
                if (!currentScene->GetWorld().Contains<Transform>(entity)) {
                    currentScene->GetWorld().Add<Transform>(entity, Transform{});
                }
            }

            if (ImGui::MenuItem("Material")) {
                if (!currentScene->GetWorld().Contains<Material>(entity)) {
                    currentScene->GetWorld().Add<Material>(entity, Material{});
                }
            }

            if (ImGui::MenuItem("Mesh")) {
                if (!currentScene->GetWorld().Contains<Mesh>(entity)) {
                    auto [mesh]{ currentScene->GetWorld().Add<Mesh>(entity, Mesh{}) };
                    mesh.Load(vk::Context::Get()->GetDataDirectory() + "Assets/Models/cube.obj");
                }
            }

            if (ImGui::BeginMenu("RigidBody")) {
                bool isStatic = false;
                PhysicsColliderShape colliderShape{ PhysicsColliderShape::eInvalid };
                if (ImGui::BeginMenu("Dynamic")) {
                    if (ImGui::MenuItem("Box Collider")) {
                        colliderShape = PhysicsColliderShape::eBox;
                    }
                    if (ImGui::MenuItem("Sphere Collider")) {
                        colliderShape = PhysicsColliderShape::eSphere;
                    }
                    if (ImGui::MenuItem("Capsule Collider")) {
                        colliderShape = PhysicsColliderShape::eCapsule;
                    }
                    if (ImGui::MenuItem("Mesh collider")) {
                        colliderShape = PhysicsColliderShape::eMesh;
                    }
                    if (ImGui::MenuItem("Convex mesh collider")) {
                        colliderShape = PhysicsColliderShape::eConvexMesh;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Static")) {
                    if (ImGui::MenuItem("Box Collider")) {
                        colliderShape = PhysicsColliderShape::eBox;
                        isStatic      = true;
                    }
                    if (ImGui::MenuItem("Sphere Collider")) {
                        colliderShape = PhysicsColliderShape::eSphere;
                        isStatic      = true;
                    }
                    if (ImGui::MenuItem("Capsule Collider")) {
                        colliderShape = PhysicsColliderShape::eCapsule;
                        isStatic      = true;
                    }
                    if (ImGui::MenuItem("Mesh collider")) {
                        colliderShape = PhysicsColliderShape::eMesh;
                        isStatic      = true;
                    }
                    if (ImGui::MenuItem("Convex mesh collider")) {
                        colliderShape = PhysicsColliderShape::eConvexMesh;
                        isStatic      = true;
                    }
                    ImGui::EndMenu();
                }
                if (colliderShape != PhysicsColliderShape::eInvalid &&
                    !currentScene->GetWorld().Contains<RigidBody>(entity) &&
                    currentScene->GetWorld().Contains<Transform>(entity)) {
                    auto [rigidBody]{ currentScene->GetWorld().Add<RigidBody>(entity, RigidBody{}) };
                    auto [transform]{ currentScene->GetWorld().Get<Transform>(entity) };

                    Mesh* meshPtr{ nullptr };
                    if ((colliderShape == PhysicsColliderShape::eMesh || colliderShape == PhysicsColliderShape::eConvexMesh) &&
                        currentScene->GetWorld().Contains<Mesh>(entity)) {
                        auto [mesh]{ currentScene->GetWorld().Get<Mesh>(entity) };
                        meshPtr = &mesh;
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
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Script")) {
                if (!currentScene->GetWorld().Contains<lua::Script>(entity)) {
                    scriptInputName = true;
                }
            }

            if (ImGui::MenuItem("Camera2D")) {
                if (!currentScene->GetWorld().Contains<Camera2D>(entity)) {
                    currentScene->GetWorld().Add<Camera2D>(entity, Camera2D{});
                }
            }

            if (ImGui::MenuItem("Camera3D")) {
                if (!currentScene->GetWorld().Contains<Camera3D>(entity)) {
                    currentScene->GetWorld().Add<Camera3D>(entity, Camera3D{});
                }
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        if (scriptInputName) {
            ImGui::OpenPopup("Input new script name:");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Input new script name:", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Separator();
                static char buffer[256];
                std::string buf;
                ImGui::InputText("##Tag", buffer, 256u);
                buf = buffer;
                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    if (buf.empty()) {
                        buf = "Default";
                    }
                    ImGui::CloseCurrentPopup();
                    scriptInputName = false;
#if defined(ADH_APPLE)
                    auto scriptDir   = vk::Context::Get()->GetDataDirectory();
                    std::string cmd1 = "cp " + scriptDir + "Resources/Scripts/template.lua " +
                                       scriptDir + "Assets/Scripts/" + buf + ".lua";
                    std::string cmd2 = "open " + scriptDir + "Assets/Scripts/" + buf + ".lua";
                    system(cmd1.data());
                    system(cmd2.data());
#elif defined(ADH_WINDOWS)
                    auto scriptDir   = vk::Context::Get()->GetDataDirectory();
                    std::string cmd1 = "copy " + scriptDir + "Resources/Scripts/template.lua " +
                                       scriptDir + "Assets/Scripts/" + buf + ".lua";
                    std::string cmd2 = "start " + scriptDir + "Assets/Scripts/" + buf + ".lua";

                    for (int i{}; i != cmd1.size(); ++i) {
                        if (cmd1[i] == '/') {
                            cmd1[i] = '\\';
                        }
                    }

                    for (int i{}; i != cmd2.size(); ++i) {
                        if (cmd2[i] == '/') {
                            cmd2[i] = '\\';
                        }
                    }
                    system(cmd1.data());
                    system(cmd2.data());
#endif
                    std::memset(buffer, 0, 256);

                    currentScene->GetWorld().Add<lua::Script>(entity, lua::Script(
                                                                          currentScene->GetState().CreateScript(
                                                                              scriptDir + "Assets/Scripts/" + buf + ".lua")));
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                    scriptInputName = false;
                }
                ImGui::EndPopup();
            }
        }
    }
} // namespace adh
