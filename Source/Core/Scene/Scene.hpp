#pragma once

#include <Std/UniquePtr.hpp>

#include <Physics/PhysicsWorld.hpp>

#include <Entity/Entity.hpp>
#include <Scripting/Script.hpp>
#include <string>

#include "Serializer.hpp"

namespace adh {
    class Scene {
        friend class Serializer;

      public:
        Scene(std::string tag = "Untitled");

        ecs::World& GetWorld();

        const ecs::World& GetWorld() const;

        lua::State& GetState();

        const lua::State& GetState() const;

        PhysicsWorld& GetPhysics();

        const PhysicsWorld& GetPhysics() const;

        void ResetPhysicsWorld();

        const std::string& GetTag() const noexcept;

        void SetTag(std::string newTag);

        void Save();

        void SaveToFile(const char* filePath);

        void Load();

        void LoadFromFile(const char* filePath);

      private:
        std::string m_Tag;
        PhysicsWorld m_PhysicsWorld;
        lua::State m_State;
        ecs::World m_World;
        Serializer m_Serializer;
    };
} // namespace adh
