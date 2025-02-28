#pragma once

#include <Entity/Entity.hpp>
#include <Std/Array.hpp>
#include <functional>
#include <string>

struct lua_State;

namespace adh {
    class Input;
    class Scene;

    struct ScriptHandler {
        static int GetScene(lua_State* L);

        static int LoadScene(lua_State* L);

        static int CreateEntity(lua_State* L);

        static int DestroyEntity(lua_State* L);

        static int AddComponent(lua_State* L);

        static int RemoveComponent(lua_State* L);

        static int GetComponent(lua_State* L);

        static int GetInput(lua_State* L);

        static int GetThis(lua_State* L);

        static int DeltaTime(lua_State* L);

        static int LogMessage(lua_State* L);

        static int LogError(lua_State* L);

        static int Raycast(lua_State* L);

        static int SetGravity(lua_State* L);

        static int FindEntity(lua_State* L);

        static int CallScript(lua_State* L);

        static int SerializeField(lua_State* L);

        static void RegisterBindings();

        inline static Scene* scene;
        inline static Input* input;
        inline static Array<std::string> debugLog;
        inline static Array<std::string> errorLog;
        inline static std::uint64_t currentEntity;
        inline static float deltaTime;

        inline static const char* loadSceneFilename{ nullptr };

        inline static Array<std::function<void()>> scriptComponentEvent;
    };
} // namespace adh
