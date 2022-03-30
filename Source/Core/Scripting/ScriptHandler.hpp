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

#include <Std/Array.hpp>
#include <string>

struct lua_State;

namespace adh {
    class Input;
    class Scene;

    struct ScriptHandler {
        static int CreateEntity(lua_State* L);

        static int DestroyEntity(lua_State* L);

        static int AddComponent(lua_State* L);

        static int RemoveComponent(lua_State* L);

        static int GetComponent(lua_State* L);

        static int GetInput(lua_State* L);

        static int GetThis(lua_State* L);

        static int DeltaTime(lua_State* L);

        // static int DispatchEvent(lua_State* L);

        static int LogMessage(lua_State* L);

        static int LogError(lua_State* L);

        static int Raycast(lua_State* L);

        // static int RaycastAll(lua_State* L);

        static void RegisterBindings();

        inline static Scene* scene;
        inline static Input* input;
        inline static Array<std::string> debugLog;
        inline static Array<std::string> errorLog;
        inline static std::uint64_t currentEntity;
        inline static float deltaTime;
    };
} // namespace adh
