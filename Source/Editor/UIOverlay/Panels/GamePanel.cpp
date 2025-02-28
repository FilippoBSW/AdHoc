#include "GamePanel.hpp"
#include "ViewportUtility.hpp"
#include <ImGui/imgui.h>

namespace adh {
    void GamePanel::Draw(void* viewportID, float aspectRatio) {
        if (isOpen) {
            if (!ImGui::Begin("Game", &isOpen)) {
                ImGui::End();
            }

            else {
                if (aspectRatio == 1.0f) {
                    ImGui::Image(viewportID, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
                }

                else {
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
                }

                isFocused = ImGui::IsWindowFocused();

                ImGui::End();
            }
        }
    }
} // namespace adh
