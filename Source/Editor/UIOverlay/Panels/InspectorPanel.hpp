#pragma once
#include <Entity/Entity.hpp>
#include <Scene/Scene.hpp>

namespace adh {
    class InspectorPanel {
      public:
        void Draw(Scene* scene, ecs::Entity& entity);

      private:
        void DrawComponents(ecs::Entity& entity);

        void AddComponent(ecs::Entity& entity);

      public:
        Scene* currentScene;
        ecs::Entity* selectedEntity;
        bool isOpen{ true };
    };
} // namespace adh
