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
