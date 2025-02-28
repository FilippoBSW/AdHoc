#include "ControlsPanel.hpp"
#include <ImGui/imgui.h>

namespace adh {

    void ControlsPanel::Draw(bool* drawEditor, bool* play) {
        if (isOpen) {
            if (!ImGui::Begin("Controls", &isOpen)) {
                ImGui::End();
            }

            else {
                ImGui::Checkbox("Play", play);
                ImGui::Checkbox("Maximize on play", drawEditor);
                ImGui::End();
            }
        }
    }

} // namespace adh
