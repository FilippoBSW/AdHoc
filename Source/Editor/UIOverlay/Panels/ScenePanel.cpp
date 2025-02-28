#include "ScenePanel.hpp"
#include "ViewportUtility.hpp"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <iostream>

namespace adh {
    void ScenePanel::Draw(Scene* scene, ecs::Entity entity, GuizmoMode guizmoMode, void* viewportID, float aspectRatio) {
        if (isOpen) {
            if (!ImGui::Begin("Scene", &isOpen)) {
                ImGui::End();
            } else {
                if (aspectRatio == 1.0f) {
                    ImGui::Image(viewportID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
                } else {
                    ImVec2 imageViewPort{ GetLargestViewportSize(aspectRatio) };
                    ImVec2 centerPos{ GetCenterPos(imageViewPort) };
                    ImGui::SetCursorPos(centerPos);
                    auto cursorScreenPos{ ImGui::GetCursorScreenPos() };
                    cursorScreenPos.x -= ImGui::GetScrollX();
                    cursorScreenPos.y -= ImGui::GetScrollY();
                    rect.left   = cursorScreenPos.x;
                    rect.top    = cursorScreenPos.y;
                    rect.right  = cursorScreenPos.x + imageViewPort.x;
                    rect.bottom = cursorScreenPos.y + imageViewPort.y;
                    ImGui::Image(viewportID, imageViewPort, ImVec2(0, 1), ImVec2(1, 0));

                    // Guizmo begin
                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();
                    ImGuizmo::SetRect(cursorScreenPos.x, cursorScreenPos.y, imageViewPort.x, imageViewPort.y);

                    xmm::Matrix cameraView;
                    xmm::Matrix cameraProj;
                    scene->GetWorld().GetSystem<Camera3D>().ForEach([&](Camera3D& camera) {
                        if (camera.isSceneCamera) {
                            cameraView = camera.GetXmmView();
                            cameraProj = camera.GetXmmProjection();
                            return;
                        }
                    });

                    Transform* transform = nullptr;
                    xmm::Matrix entityTransform;
                    if (scene->GetWorld().Contains<Transform>(entity)) {
                        auto [t]        = scene->GetWorld().Get<Transform>(entity);
                        entityTransform = t.GetXmm();
                        transform       = &t;
                    }

                    ImGuizmo::OPERATION operation;
                    switch (guizmoMode) {
                    case GuizmoMode::eTranslate:
                        {
                            operation = ImGuizmo::OPERATION::TRANSLATE;
                            break;
                        }
                    case GuizmoMode::eRotate:
                        {
                            operation = ImGuizmo::OPERATION::ROTATE;
                            break;
                        }
                    case GuizmoMode::eScale:
                        {
                            operation = ImGuizmo::OPERATION::SCALE;
                            break;
                        }
                    }

                    ImGuizmo::Manipulate((float*)&cameraView.m, (float*)&cameraProj, operation, ImGuizmo::LOCAL, (float*)&entityTransform.m);
                    if (ImGuizmo::IsUsing()) {
                        if (transform) {
                            Vector3D t, r, s;
                            entityTransform.Decompose(t, r, s);
                            Vector3D deltaRotation = r - transform->rotation;
                            transform->translate   = t;
                            transform->rotation += deltaRotation;
                            transform->scale = s;
                        }
                    }
                    // End guizmo

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL")) {
                            const char* buffer = (const char*)payload->Data;
                            auto type          = strrchr(buffer, '.');

                            if (!strcmp(type, ".scene")) {
                                Event::Dispatch<StatusEvent>(StatusEvent::Type::eStop);
                                scene->LoadFromFile(buffer);
                            }

                            ImGui::EndDragDropTarget();
                        }
                    }
                    isFocused = ImGui::IsWindowFocused();

                    ImGui::End();
                }
            }
        }
    }
} // namespace adh
