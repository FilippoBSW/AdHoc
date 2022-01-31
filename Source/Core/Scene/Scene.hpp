// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
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

#pragma once

#include <Std/UniquePtr.hpp>

#include <Physics/Physics.hpp>

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

        Physics& GetPhysics();

        const Physics& GetPhysics() const;

        void ResetPhysicsWorld();

        const std::string& GetTag() const noexcept;

        void SetTag(std::string newTag);

        void Save();

        void SaveToFile(const char* filePath);

        void Load();

        void LoadFromFile(const char* filePath);

      private:
        std::string m_Tag;
        Physics m_PhysicsWorld;
        ecs::World m_World;
        lua::State m_State;
        Serializer m_Serializer;
    };
} // namespace adh
