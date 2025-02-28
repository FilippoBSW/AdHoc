#pragma once
#include <Entity/Entity.hpp>
#include <Scene/Scene.hpp>

namespace adh {
    struct Tag;

    class SceneHierarchyPanel {
      public:
        void Draw(Scene* scene, ecs::Entity& selectedEntity) noexcept;

      private:
        void DrawEntities(ecs::Entity& entity, Tag& tag) noexcept;

      public:
        Scene* currentScene;
        ecs::Entity* selectedEntity;
        bool isOpen{ true };
    };
} // namespace adh
