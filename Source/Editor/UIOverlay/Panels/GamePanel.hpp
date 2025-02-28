#pragma once
#include "ViewportRect.hpp"

namespace adh {
    class GamePanel {
      public:
        void Draw(void* viewportID, float aspectRatio);

      public:
        ViewportRect rect;
        bool isOpen{ true };
        bool isFocused{ false };
    };
} // namespace adh
