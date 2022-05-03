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

#include <lua.hpp>

#include <atomic>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

#include <Entity/Entity.hpp>
#include <Event/Event.hpp>

#include <Std/Array.hpp>
#include <Std/Function.hpp>
#include <Std/UniquePtr.hpp>
#include <Std/Utility.hpp>
#include <Utility.hpp>

#include <Math/Math.hpp>

#include <Scene/Components.hpp>

namespace adh {
    namespace lua {

#if defined(ADH_WINDOWS)
#    define ADH_LUA_UPDATE_STACK_INDEX(x) x--
#else
#    define ADH_LUA_UPDATE_STACK_INDEX(x) x++
#endif
        template <typename T>
        struct GetType {
        };

        template <typename T>
        struct GetType<T*> {
            T* operator()(lua_State* L, std::int32_t& index) {
                return *(T**)lua_touserdata(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <typename T>
        struct GetType<const T*> {
            const T* operator()(lua_State* L, std::int32_t& index) {
                return *(T**)lua_touserdata(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <typename T>
        struct GetType<const T* const> {
            const T* const operator()(lua_State* L, std::int32_t& index) {
                return *(T**)lua_touserdata(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <typename T>
        struct GetType<T&> {
            T& operator()(lua_State* L, std::int32_t& index) {
                return **(T**)lua_touserdata(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <typename T>
        struct GetType<const T&> {
            const T& operator()(lua_State* L, std::int32_t& index) {
                return **(T**)lua_touserdata(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<float> {
            float operator()(lua_State* L, std::int32_t& index) {
                return lua_tonumber(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<double> {
            double operator()(lua_State* L, std::int32_t& index) {
                return lua_tonumber(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<std::int32_t> {
            std::int32_t operator()(lua_State* L, std::int32_t& index) {
                return lua_tointeger(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<std::uint32_t> {
            std::int32_t operator()(lua_State* L, std::int32_t& index) {
                return lua_tointeger(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<std::int64_t> {
            std::int64_t operator()(lua_State* L, std::int32_t& index) {
                return lua_tointeger(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<std::uint64_t> {
            std::int64_t operator()(lua_State* L, std::int32_t& index) {
                return lua_tointeger(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<bool> {
            bool operator()(lua_State* L, std::int32_t& index) {
                return lua_toboolean(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <>
        struct GetType<const char*> {
            const char* operator()(lua_State* L, std::int32_t& index) {
                return lua_tostring(L, ADH_LUA_UPDATE_STACK_INDEX(index));
            }
        };

        template <typename T>
        struct PushValue {
        };

        template <>
        struct PushValue<float> {
            void operator()(lua_State* L, float value) {
                lua_pushnumber(L, value);
            }
        };

        template <>
        struct PushValue<double> {
            void operator()(lua_State* L, double value) {
                lua_pushnumber(L, value);
            }
        };

        template <>
        struct PushValue<std::int32_t> {
            void operator()(lua_State* L, std::int32_t value) {
                lua_pushinteger(L, value);
            }
        };

        template <>
        struct PushValue<std::uint32_t> {
            void operator()(lua_State* L, std::uint32_t value) {
                lua_pushinteger(L, value);
            }
        };

        template <>
        struct PushValue<std::int64_t> {
            void operator()(lua_State* L, std::int64_t value) {
                lua_pushinteger(L, value);
            }
        };

        template <>
        struct PushValue<std::uint64_t> {
            void operator()(lua_State* L, std::uint64_t value) {
                lua_pushinteger(L, value);
            }
        };

        template <>
        struct PushValue<bool> {
            void operator()(lua_State* L, bool value) {
                lua_pushboolean(L, value);
            }
        };

        template <>
        struct PushValue<const char*> {
            void operator()(lua_State* L, const char* value) {
                lua_pushstring(L, value);
            }
        };

        template <typename... Args>
        struct Constructor {
        };

        struct BaseType {
            virtual ~BaseType() = default;
        };

        template <typename T>
        struct Type : BaseType {
            static std::int32_t Index(lua_State* L) {
                T** ptr         = static_cast<T**>(lua_touserdata(L, 1));
                const char* key = lua_tostring(L, 2);

                if (indexMap.find(key) != indexMap.end()) {
                    indexMap[key].first(L, *ptr);
                } else if (methodsMap.find(key) != methodsMap.end()) {
                    toCall = &methodsMap[key];
                    lua_getglobal(L, typeName.data());
                    lua_pushstring(L, key);
                    lua_rawget(L, -2);
                }

                return 1;
            }

            static std::int32_t NewIndex(lua_State* L) {
                T* ptr          = *static_cast<T**>(lua_touserdata(L, 1));
                const char* key = lua_tostring(L, 2);

                if (indexMap.find(key) != indexMap.end()) {
                    indexMap[key].second(L, ptr);
                }

                return 1;
            }

            static std::int32_t Constructor(lua_State* L) {
                return constructorsMap[lua_tostring(L, lua_upvalueindex(1))](L);
            }

            static std::int32_t Destructor(lua_State* L) {
                T** ptr{ static_cast<T**>(lua_touserdata(L, -1)) };
                if (auto itr = toDestroy.find(*ptr); itr != toDestroy.end()) {
                    delete *ptr;
                    toDestroy.erase(itr);
                }
                return 0;
            }

            template <typename T2>
            static void AddVariable(const char* key, T2 var) {
                indexMap[key].first = [=](lua_State* L, T* ptr) {
                    PushValue<std::decay_t<decltype(ptr->*var)>>()(L, ptr->*var);
                };

                indexMap[key].second = [=](lua_State* L, T* ptr) {
                    // stack 1 = userdata
                    // stack 2 = key
                    // stack 3 = value
                    std::int32_t index{ 3 };
                    ptr->*var = GetType<std::decay_t<decltype(ptr->*var)>>()(L, index);
                };
            }

            template <typename T2, typename R>
            static void AddVariable2(const char* key, R T2::*var) {
                indexMap[key].first = [=](lua_State* L, T* ptr) {
                    void** ptr2 = static_cast<void**>(lua_newuserdata(L, sizeof(void*)));
                    *ptr2       = &(ptr->*var);
                    luaL_getmetatable(L, Type<R>::metatableName.data());
                    lua_setmetatable(L, -2);
                };

                indexMap[key].second = [=](lua_State* L, T* ptr) {
                    // stack 1 = userdata
                    // stack 2 = key
                    // stack 3 = value
                    void** p  = static_cast<void**>(lua_touserdata(L, 3));
                    ptr->*var = *(((R*)(*p)));
                };
            }

            template <typename... Args>
            static void AddConstructor(const char* key) {
                constructorsMap[key] = [=](lua_State* L) {
                    std::int32_t index{ GetIndex(L) };
                    T* p    = new T(GetType<Args>()(L, index)...);
                    T** ptr = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
                    *ptr    = p;

                    toDestroy.emplace(p, true);
                    luaL_getmetatable(L, metatableName.data());
                    lua_setmetatable(L, -2);
                    return 1;
                };
            }

            template <typename R, typename... Args>
            static void AddMethod(const char* key, const char* retTable, R (T::*method)(Args...)) {
                methodsMap[key] = [=](lua_State* L) {
                    std::int32_t index{ GetIndex(L) };

                    T** p = static_cast<T**>(lua_touserdata(L, 1));
                    ADH_THROW(p != nullptr, "Class ptr is null!");

                    if constexpr (std::is_void<R>()) {
                        std::apply(
                            method, std::tuple_cat(std::forward_as_tuple(*p),
                                                   std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...)));
                        return 0;
                    } else {
                        auto&& retVal = std::apply(
                            method, std::tuple_cat(std::forward_as_tuple(*p),
                                                   std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...)));

                        if constexpr (std::is_pointer<R>::value || std::is_reference<R>::value) {
                            auto** ptr = static_cast<std::decay_t<R>**>(lua_newuserdata(L, sizeof(std::decay_t<R>**)));
                            *ptr       = &retVal;
                            luaL_getmetatable(L, retTable);
                            lua_setmetatable(L, -2);
                        } else if constexpr (std::is_class<R>::value) {
                            auto** ptr = static_cast<std::decay_t<R>**>(lua_newuserdata(L, sizeof(std::decay_t<R>**)));
                            R* p       = new R{ retVal };
                            *ptr       = p;
                            luaL_getmetatable(L, retTable);
                            lua_setmetatable(L, -2);
                        } else {
                            PushValue<R>()(L, retVal);
                        }

                        return 1;
                    }
                };
            }

            template <typename R, typename... Args>
            static void AddMethod(const char* key, const char* retTable, R (T::*method)(Args...) const) {
                methodsMap[key] = [=](lua_State* L) {
                    std::int32_t index{ GetIndex(L) };

                    T** p = static_cast<T**>(lua_touserdata(L, 1));
                    ADH_THROW(p != nullptr, "Class ptr is null!");

                    if constexpr (std::is_void<R>()) {
                        std::apply(
                            method, std::tuple_cat(std::forward_as_tuple(*p),
                                                   std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...)));
                        return 0;
                    } else {
                        auto&& retVal = std::apply(
                            method, std::tuple_cat(std::forward_as_tuple(*p),
                                                   std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...)));

                        if constexpr (std::is_pointer<R>::value || std::is_reference<R>::value) {
                            auto** ptr = static_cast<std::decay_t<R>**>(lua_newuserdata(L, sizeof(std::decay_t<R>**)));
                            *ptr       = &retVal;
                            luaL_getmetatable(L, retTable);
                            lua_setmetatable(L, -2);
                        } else if constexpr (std::is_class<R>::value) {
                            auto** ptr = static_cast<std::decay_t<R>**>(lua_newuserdata(L, sizeof(std::decay_t<R>**)));
                            R* p       = new R{ retVal };
                            *ptr       = p;
                            luaL_getmetatable(L, retTable);
                            lua_setmetatable(L, -2);
                        } else {
                            PushValue<R>()(L, retVal);
                        }
                        return 1;
                    }
                };
            }

            static int Dispatch(lua_State* L) {
                return (*toCall)(L);
            }

            static std::int32_t GetIndex(lua_State* L) {
#if defined(ADH_WINDOWS)
                return lua_gettop(L);
#else
                return 2;
#endif
            }

            inline static std::string typeName;
            inline static std::string metatableName;
            inline static std::atomic<std::function<int(lua_State*)>*> toCall;
            inline static std::unordered_map<std::string, std::pair<std::function<void(lua_State* L, T* ptr)>, std::function<void(lua_State* L, T* ptr)>>> indexMap;
            inline static std::unordered_map<std::string, std::function<int(lua_State*)>> methodsMap;
            inline static std::unordered_map<std::string, std::function<int(lua_State*)>> constructorsMap;
            inline static std::unordered_map<void*, bool> toDestroy;
        };

        class Script {
          public:
            Script() = default;

            Script(std::string id, lua_State* state, const std::string& filePath)
                : m_Id{ Move(id) },
                  m_State{ state } {
                auto pos       = filePath.find_last_of('/') + 1;
                fileName       = filePath.substr(pos);
                this->filePath = filePath;
            }

            Script(std::string id, lua_State* state, ecs::Entity* entity)
                : m_Id{ Move(id) },
                  m_State{ state },
                  m_Entity{ entity } {}

            void Run() {
                lua_getglobal(m_State, m_Id.data());
                if (lua_pcall(m_State, 0, LUA_MULTRET, 0) != LUA_OK) {
                    for (int i{ lua_gettop(m_State) }; i != 0; --i) {
                        if (lua_isstring(m_State, i)) {
                            std::string message = lua_tostring(m_State, i);
                            auto pos            = message.find_last_of(':') + 1;
                            std::string err     = message.substr(pos);
                            auto pos2           = m_Id.find_last_of('-');
                            auto filename       = m_Id.substr(pos2 + 1);
                            errorText           = "File: " + filename + " - " + err + "\n ";
                            Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, errorText.data());
                        }
                    }
                    lua_pop(m_State, lua_gettop(m_State));
                }
            }

            void Bind() noexcept {
                lua_getfield(m_State, LUA_REGISTRYINDEX, m_Id.data());
            }

            void Unbind() noexcept {
                lua_pop(m_State, 1);
            }

            template <typename... Args>
            void Call(const char* funcName, Args&&... args) {
                lua_getfield(m_State, -1, funcName);
                if (lua_isfunction(m_State, -1)) {
                    (PushValue<std::decay_t<decltype(args)>>()(m_State, args), ...);
                    if (lua_pcall(m_State, sizeof...(args), LUA_MULTRET, 0) != LUA_OK) {
                        std::string message = lua_tostring(m_State, -1);
                        auto pos            = message.find_last_of(':') + 1;
                        std::string err     = message.substr(pos);
                        auto pos2           = m_Id.find_last_of('-');
                        auto filename       = m_Id.substr(pos2 + 1);
                        errorText           = "File: " + filename + " - Function: " + funcName + " -> " + err + "\n ";
                        Event::Dispatch<EditorLogEvent>(EditorLogEvent::Type::eError, errorText.data());
                    }
                } else {
                    lua_pop(m_State, 1);
                }
            }

          public:
            std::string fileName;
            std::string filePath;
            std::string errorText;
            CollisionEvent collisionEvent{ CollisionEvent::Type::eCollisionInvalid, 0, 0 };
            float fixedUpdateAcculumator{};

          private:
            std::string m_Id;
            lua_State* m_State;
            ecs::Entity* m_Entity;
        };

        class State {
          public:
            State() noexcept : m_State{} {}

            State(lua_State* L) : m_State{ L } {}

            State(const State& rhs) = delete;

            State& operator=(const State& rhs) = delete;

            State(State&& rhs) noexcept {
                MoveConstruct(Move(rhs));
            }

            State& operator=(State&& rhs) noexcept {
                Clear();
                MoveConstruct(Move(rhs));
                return *this;
            }

            ~State() {
                Clear();
            }

            void Destroy() noexcept {
                Clear();
            }

            void ClearStack() noexcept {
                lua_settop(m_State, 0);
            }

            operator lua_State*() {
                return m_State;
            }

            operator const lua_State*() const {
                return m_State;
            }

            template <typename R, typename... Args>
            void RegisterFunction(const char* funcName, R (*funcPtr)(Args...)) {
                lua_pushstring(m_State, funcName);
                lua_pushcclosure(m_State, DispatchGlobal, 1);
                lua_setglobal(m_State, funcName);

                m_GlobalFunctions[funcName] = [=](lua_State* L) {
                    std::int32_t index{ 1 };
                    if constexpr (std::is_void<R>()) {
                        std::apply(funcPtr, std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...));
                        return 0;
                    } else {
                        auto&& retVal = std::apply(funcPtr, std::forward_as_tuple(static_cast<Args>(GetType<Args>()(L, index))...));
                        PushValue<R>()(L, retVal);
                        if constexpr (std::is_pointer<R>::value || std::is_reference<R>::value) {
                            luaL_getmetatable(L, GetPointer<R>()->metatableName.data());
                            lua_setmetatable(L, -2);
                        }
                        return 1;
                    }
                };
            }

            template <typename T, typename... Args>
            void RegisterType(std::string typeName, Args&&... args) {
                auto& type              = m_Types.EmplaceBack(MakeUnique<Type<T>>());
                auto* realType          = static_cast<Type<T>*>(type.Get());
                std::string metatable   = typeName + "Metatable";
                realType->typeName      = Move(typeName);
                realType->metatableName = Move(metatable);

                luaL_newmetatable(m_State, realType->metatableName.data());
                lua_pushcfunction(m_State, Type<T>::Index);
                lua_setfield(m_State, -2, "__index");
                lua_pushcfunction(m_State, Type<T>::NewIndex);
                lua_setfield(m_State, -2, "__newindex");
                lua_pushcfunction(m_State, Type<T>::Destructor);
                lua_setfield(m_State, -2, "__gc");
                lua_pop(m_State, 1);

                lua_newtable(m_State);
                lua_pushvalue(m_State, -1);
                lua_setglobal(m_State, realType->typeName.data());
                lua_pop(m_State, 1);
            }

            template <typename T, typename F, typename = std::enable_if_t<!std::is_class<F>::value>>
            void RegisterTypeVariable(const char* key, F func) {
                GetPointer<T>()->AddVariable(key, func);
            }

            template <typename T, typename R, typename = std::enable_if_t<std::is_class<R>::value>>
            void RegisterTypeVariable(const char* key, R T::*func) {
                GetPointer<T>()->template AddVariable2<T, R>(key, func);
            }

            template <typename T, typename... Args>
            void RegisterTypeConstructor(const char* key) {
                lua_getglobal(m_State, GetPointer<T>()->typeName.data());
                lua_pushstring(m_State, key);
                lua_pushcclosure(m_State, Type<T>::Constructor, 1);
                lua_setfield(m_State, -2, key);
                GetPointer<T>()->template AddConstructor<Args...>(key);
                lua_pop(m_State, 1);
            }

            template <typename T, class R, typename... Args>
            void RegisterTypeFunction(const char* key, R (T::*method)(Args...)) {
                lua_getglobal(m_State, GetPointer<T>()->typeName.data());
                lua_pushcfunction(m_State, Type<T>::Dispatch);
                lua_setfield(m_State, -2, key);
                lua_pop(m_State, 1);

                const char* retMetatable{ "Null" };
                if constexpr (std::is_class<std::decay_t<R>>::value) {
                    retMetatable = GetPointer<R>()->metatableName.data();
                }
                GetPointer<T>()->template AddMethod<R, Args...>(key, retMetatable, method);
            }

            template <typename T, class R, typename... Args>
            void RegisterTypeFunction(const char* key, R (T::*method)(Args...) const) {
                lua_getglobal(m_State, GetPointer<T>()->typeName.data());
                lua_pushcfunction(m_State, Type<T>::Dispatch);
                lua_setfield(m_State, -2, key);
                lua_pop(m_State, 1);

                const char* retMetatable{ "Null" };
                if constexpr (std::is_class<std::decay_t<R>>::value) {
                    retMetatable = GetPointer<R>()->metatableName.data();
                }
                GetPointer<T>()->template AddMethod<R, Args...>(key, retMetatable, method);
            }

            template <typename T>
            int BindObject(T* obj) {
                T** ptr = static_cast<T**>(lua_newuserdata(m_State, sizeof(T*)));
                *ptr    = obj;
                luaL_getmetatable(m_State, GetPointer<T>()->metatableName.data());
                lua_setmetatable(m_State, -2);
                return 1;
            }

            [[nodiscard]] Script CreateScript(const std::string& script) {
                return CreateScript(script, uniqueId++);
            }

            [[nodiscard]] Script CreateScript2(const char* script) {
                return CreateScript2(script, uniqueId++);
            }

            [[nodiscard]] Script CreateScript(const std::string& script, ecs::Entity* entity) {
                return CreateScript(script, static_cast<std::uint64_t>(*entity));
            }

          private:
            Script CreateScript(const std::string& script, std::uint64_t id) {
                auto uniqueScript{ CreateUniqueScript(script, id) };
                luaL_loadfile(m_State, script.data());
                lua_newtable(m_State);
                lua_newtable(m_State);
                lua_getglobal(m_State, "_G");
                lua_setfield(m_State, -2, "__index");
                lua_setmetatable(m_State, -2);
                lua_setfield(m_State, LUA_REGISTRYINDEX, uniqueScript.data());
                lua_getfield(m_State, LUA_REGISTRYINDEX, uniqueScript.data());
                lua_setupvalue(m_State, 1, 1);
                lua_setglobal(m_State, uniqueScript.data());
                return Script(Move(uniqueScript), m_State, script);
            }

            Script CreateScript2(const char* script, std::uint64_t id) {
                auto uniqueScript{ CreateUniqueScript("Script.lua", id) };
                luaL_loadstring(m_State, script);
                lua_newtable(m_State);
                lua_newtable(m_State);
                lua_getglobal(m_State, "_G");
                lua_setfield(m_State, -2, "__index");
                lua_setmetatable(m_State, -2);
                lua_setfield(m_State, LUA_REGISTRYINDEX, uniqueScript.data());
                lua_getfield(m_State, LUA_REGISTRYINDEX, uniqueScript.data());
                lua_setupvalue(m_State, 1, 1);
                lua_setglobal(m_State, uniqueScript.data());
                return Script(Move(uniqueScript), m_State, "Script.lua");
            }

            std::string CreateUniqueScript(const std::string& script, std::uint64_t id) {
                auto pos          = script.find_last_of('/');
                auto scriptName   = script.substr(pos + 1);
                auto uniqueScript = std::to_string(id) + "-" + scriptName;
                return uniqueScript;
            }

            void MoveConstruct(State&& rhs) noexcept {
                m_State     = rhs.m_State;
                rhs.m_State = nullptr;
            }

            void Clear() noexcept {
                if (m_State) {
                    lua_close(m_State);
                    m_Types.Clear();
                    m_GlobalFunctions.clear();
                }
            }

            template <typename T>
            std::uint64_t GetIndex() {
                static std::uint64_t index = m_Types.GetSize() - 1u;
                return index;
            }

            template <typename T>
            auto GetPointer() {
                return static_cast<Type<ToPureType<T>>*>(m_Types[GetIndex<ToPureType<T>>()].Get());
            }

            static std::int32_t DispatchGlobal(lua_State* L) {
                return m_GlobalFunctions[lua_tostring(L, lua_upvalueindex(1))](L);
            }

          private:
            lua_State* m_State;
            inline static std::unordered_map<std::string, std::function<int(lua_State*)>> m_GlobalFunctions;
            Array<UniquePtr<BaseType>> m_Types;
            inline static std::uint64_t uniqueId;
        };

        inline State NewState() {
            State state{ luaL_newstate() };
            if (state) {
                luaL_openlibs(state);

#if defined(ADH_APPLE)
                luaL_loadstring(state, R"(

                AdHoc = {
                    Key = {
						a            = 0x00,
    	                s            = 0x01,
    	                d            = 0x02,
    	                f            = 0x03,
    	                h            = 0x04,
    	                g            = 0x05,
    	                z            = 0x06,
    	                x            = 0x07,
    	                c            = 0x08,
    	                v            = 0x09,
    	                b            = 0x0B,
    	                q            = 0x0C,
    	                w            = 0x0D,
    	                e            = 0x0E,
    	                r            = 0x0F,
    	                y            = 0x10,
    	                t            = 0x11,
    	                o            = 0x1F,
    	                u            = 0x20,
    	                i            = 0x22,
    	                p            = 0x23,
    	                l            = 0x25,
    	                j            = 0x26,
    	                k            = 0x28,
    	                n            = 0x2D,
    	                m            = 0x2E,
						enter        = 0x24,
						space        = 0x31,
						command      = 0x37,
						shift        = 0x38,
						option       = 0x3A,
						control      = 0x3B,
						left         = 0x7B,
						right        = 0x7C,
						down         = 0x7D,
						up           = 0x7E },
                    Controller = {
						a            = 0x3,
                        b            = 0x2,
                        x            = 0x0,
                        y            = 0x1,
                        rshoulder    = 0x8,
                        lshoulder    = 0x9,
                        ltrigger     = 0xa,
                        rtrigger     = 0xb,
                        dpad_up      = 0x4,
                        dpad_down    = 0x5,
                        dpad_left    = 0x6,
                        dpad_right   = 0x7,
                        start        = 0xc,
                        back         = 0xd,
                        lthumb_press = 0xe,
					    rthumb_press = 0xf },
                    Global = _G }
                package.preload['AdHoc'] = function()
                	return AdHoc
                end

                )");
                ADH_THROW(lua_pcall(state, 0, LUA_MULTRET, 0) == LUA_OK, lua_tostring(state, -1));

#elif defined(ADH_WINDOWS)
                luaL_loadstring(state, R"(

                AdHoc = {
                    Key = {
                        a         = 0x41,
                        b         = 0x42,
                        c         = 0x43,
                        d         = 0x44,
                        e         = 0x45,
                        f         = 0x46,
                        g         = 0x47,
                        h         = 0x48,
                        i         = 0x49,
                        j         = 0x4A,
                        k         = 0x4B,
                        l         = 0x4C,
                        m         = 0x4D,
                        n         = 0x4E,
                        o         = 0x4F,
                        p         = 0x50,
                        q         = 0x51,
                        r         = 0x52,
                        s         = 0x53,
                        t         = 0x54,
                        u         = 0x55,
                        v         = 0x56,
                        w         = 0x57,
                        x         = 0x58,
                        y         = 0x59,
                        z         = 0x5A,
                        left      = 0x25,
                        up        = 0x26,
                        right     = 0x27,
                        down      = 0x28,
                        space     = 0x20,
                        shift     = 0x10,
                        control   = 0x11,
                        enter     = 0x0D },
                    Controller = {
                        a            = 0x5800,
                        b            = 0x5801,
                        x            = 0x5802,
                        Y            = 0x5803,
                        rshoulder    = 0x5804,
                        lshoulder    = 0x5805,
                        ltrigger     = 0x5806,
                        rtrigger     = 0x5807,
                        dpad_up      = 0x5810,
                        dpad_down    = 0x5811,
                        dpad_left    = 0x5812,
                        dpad_right   = 0x5813,
                        start        = 0x5814,
                        back         = 0x5815,
                        lthumb_press = 0x5816,
                        rthumb_press = 0x5817 },
                    Global = _G }
                package.preload['AdHoc'] = function()
                	return AdHoc
                end

                )");
                ADH_THROW(lua_pcall(state, 0, LUA_MULTRET, 0) == LUA_OK, lua_tostring(state, -1));
#endif

                return state;
            } else {
                ADH_THROW(false, "Failed to create new Lua state!");
                return State{};
            }
        }
    } // namespace lua
} // namespace adh
