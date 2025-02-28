#pragma once
#include "ViewportRect.hpp"
#include <Scene/Scene.hpp>

namespace adh {
    enum class GuizmoMode {
        eTranslate,
        eRotate,
        eScale
    };

    class ScenePanel {
      public:
        void Draw(Scene* scene, ecs::Entity entity, GuizmoMode guizmoMode, void* viewportID, float aspectRatio);

      public:
        ViewportRect rect;
        bool isOpen{ true };
        bool isFocused{ false };
    };
} // namespace adh
