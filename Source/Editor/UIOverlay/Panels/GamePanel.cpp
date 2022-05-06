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
