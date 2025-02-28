#pragma once
#include <ImGui/imgui.h>

namespace adh {
    static ImVec2 GetLargestViewportSize(float aspectRatio) {
        ImVec2 ret{ ImGui::GetContentRegionAvail() };
        ret.x -= ImGui::GetScrollX();
        ret.y -= ImGui::GetScrollY();

        float aspectWidth{ ret.x };
        float aspectHeight{ aspectWidth / aspectRatio };

        if (aspectHeight > ret.y) {
            aspectHeight = ret.y;
            aspectWidth  = aspectHeight * aspectRatio;
        }

        return ImVec2{ aspectWidth, aspectHeight };
    }

    static ImVec2 GetCenterPos(ImVec2 aspectSize) {
        ImVec2 ret{ ImGui::GetContentRegionAvail() };
        ret.x -= ImGui::GetScrollX();
        ret.y -= ImGui::GetScrollY();

        float viewportX{ (ret.x / 2.0f) - (aspectSize.x / 2.0f) };
        float viewportY{ (ret.y / 2.0f) - (aspectSize.y / 2.0f) };

        return ImVec2{ viewportX + ImGui::GetCursorPosX(), viewportY + ImGui::GetCursorPosY() };
    }
} // namespace adh
