#pragma once

namespace adh {
    class ControlsPanel {
      public:
        void Draw(bool* drawEditor, bool* play);

      public:
        bool isOpen{ false };
    };
} // namespace adh
