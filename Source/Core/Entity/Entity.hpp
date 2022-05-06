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
#include <Std/Queue.hpp>
#include <Std/SparseSet.hpp>
#include <Utility.hpp>

#include <iostream>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace adh {
    namespace ecs {
        struct Archetype;

        using TypeID      = std::uint64_t;
        using EntityID    = TypeID;
        using ComponentID = TypeID;
        using ArchetypeID = std::vector<ComponentID>;

        enum class Entity : EntityID {};
        constexpr Entity null_entity{ std::numeric_limits<EntityID>::max() };
        constexpr std::uint32_t entity_shift{ 0x00000020 };

        template <typename T>
        class TypeIDGenerator {
          public:
            template <typename T2>
            static TypeID GetID() noexcept {
                static TypeID id{ m_IndexID++ };
                return id;
            }

          private:
            inline static TypeID m_IndexID{ 0u };
        };

        struct BaseContainer {
            virtual ~BaseContainer()                                   = default;
            virtual ComponentID GetID() const noexcept                 = 0;
            virtual std::unique_ptr<BaseContainer> Create()            = 0;
            virtual void Move(std::uint32_t index, BaseContainer* ptr) = 0;
            virtual void Delete(std::uint32_t index) noexcept          = 0;
        };

        template <typename T>
        struct Container : BaseContainer {
            static constexpr std::uint32_t NullPosition = std::numeric_limits<std::uint32_t>::max();
            ComponentID GetID() const noexcept override {
                return TypeIDGenerator<ComponentID>::GetID<T>();
            }

            std::unique_ptr<BaseContainer> Create() override {
                return std::unique_ptr<BaseContainer>(new Container{});
            }
            void Move(std::uint32_t index, BaseContainer* ptr) override {
                static_cast<Container*>(ptr)->Emplace(index, std::move(data[sparse[index]]));
                Delete(index);
            }

            void Delete(std::uint32_t index) noexcept override {
                auto temp = data.size() - 1;

                if (temp != sparse[index]) {
                    data[sparse[index]]  = std::move(data[temp]);
                    auto t               = sparse[dense[temp]];
                    sparse[dense[temp]]  = sparse[index];
                    dense[sparse[index]] = dense[temp];
                }
                sparse[index] = NullPosition;

                data.pop_back();
                dense.pop_back();
            }

            template <typename... Args>
            void Emplace(std::uint32_t index, Args&&... args) {
                if (index >= sparse.size()) {
                    sparse.resize(index + 1);
                }
                data.emplace_back(std::forward<Args>(args)...);
                dense.emplace_back(index);
                sparse[index] = data.size() - 1;
            }

            decltype(auto) Get(std::uint32_t index) {
                return data[sparse[index]];
            }

            std::vector<T> data;
            std::vector<std::uint32_t> dense;
            std::vector<std::uint32_t> sparse;
        };

        struct Record {
            Archetype* archetype;
        };

        struct Edge {
            Archetype* next;
            Archetype* prev;
        };

        struct Archetype {
            std::vector<Entity> entities;
            ArchetypeID type;
            std::unordered_map<ComponentID, Edge> edges;
            SparseSet<std::unique_ptr<BaseContainer>, 4096u> components;
        };

        template <typename... Components>
        class System {
          public:
            friend class World;

          public:
            template <typename T>
            void ForEach(T func) {
                for (std::size_t j{}; j != archetypes.size(); ++j) {
                    for (std::int64_t i = archetypes[j]->entities.size() - 1u; i >= 0; --i) {
                        if constexpr (std::is_invocable_v<T, decltype(std::declval<Components&>())...>) {
                            std::apply(
                                func,
                                std::forward_as_tuple(GetPointer<Components>(archetypes[j])->data[i]...));
                        } else {
                            std::apply(
                                func,
                                std::tuple_cat(
                                    std::forward_as_tuple(archetypes[j]->entities[i]),
                                    std::forward_as_tuple(GetPointer<Components>(archetypes[j])->data[i]...)));
                        }
                    }
                }
            }

          private:
            template <typename T>
            auto GetPointer(Archetype* node) noexcept {
                return static_cast<Container<T>*>(node->components[GetID<T>()].get());
            }

            template <typename T>
            ComponentID GetID() const noexcept {
                return TypeIDGenerator<ComponentID>::GetID<T>();
            }

          private:
            std::vector<Archetype*> archetypes;
        };

        class World {
          public:
            World() : m_RootArchetype{ new Archetype{} } {
            }

            World(const World& rhs) = delete;

            World& operator=(const World& rhs) = delete;

            World(World&& rhs) noexcept {
                MoveConstruct(std::move(rhs));
            }

            World& operator=(World&& rhs) noexcept {
                Clear();
                MoveConstruct(std::move(rhs));
                return *this;
            }

            ~World() {
                Clear();
            }

            void Reset() {
                Clear();
                m_RootArchetype = new Archetype{};
            }

            Entity CreateEntity() {
                if (m_RecicledEntities.IsEmpty()) {
                    return GenerateID();
                } else {
                    return RecycleID();
                }
            }

            template <typename... T, typename... Args>
            decltype(auto) Add(Entity entity, Args&&... args) {
                Record& r{ m_EntityArchetype[entity] };
                ADH_THROW(!Contains<T...>(entity), "Entity already has component!");
                ArchetypeID archetypeID{ GetArchetypeID<T...>(r.archetype) };
                Archetype* node{ FindArchetype(archetypeID) };

                if (r.archetype) {
                    PopEntity(entity, r.archetype);
                }

                if (node->components.IsEmpty()) {
                    if (r.archetype) {
                        auto& components{ r.archetype->components };
                        auto& typeIDs{ r.archetype->type };
                        for (std::size_t i{}; i != components.GetSize(); ++i) {
                            node->components.Add(typeIDs[i], components[typeIDs[i]]->Create());
                            components[typeIDs[i]]->Move(GetIndex(GetType(entity)) - 1, node->components[typeIDs[i]].get());
                        }
                    }
                    (AddComponent<T>(node),
                     ...);
                } else {
                    if (r.archetype) {
                        auto& components{ r.archetype->components };
                        auto& typeIDs{ r.archetype->type };
                        for (std::size_t i{}; i != components.GetSize(); ++i) {
                            components[typeIDs[i]]->Move(GetIndex(GetType(entity)) - 1, node->components[typeIDs[i]].get());
                        }
                    }
                }

                if constexpr (sizeof...(Args) == sizeof...(T)) {
                    (EmplaceData<T>(node, GetIndex(GetType(entity)) - 1, std::forward<Args>(args)), ...);
                } else if constexpr (sizeof...(T) == 1) {
                    GetPointer<std::decay<T...>>(node)->Emplace(GetIndex(GetType(entity)) - 1, std::forward<Args>(args)...);
                } else {
                    throw std::runtime_error("Invalid T... and Args... combination");
                }
                r.archetype = node;
                r.archetype->entities.emplace_back(entity);
                return std::forward_as_tuple(GetPointer<T>(r.archetype)->Get(GetIndex(GetType(entity)) - 1)...);
            }

            template <typename... T>
            void Remove(Entity entity) ADH_NOEXCEPT {
                Record& r{ m_EntityArchetype[entity] };
                ADH_THROW(r.archetype, "Empty entity!");
                ADH_THROW(Contains<T...>(entity), "Entity doesn't have component!");
                ArchetypeID toRemoveIDs{ GetID<T>()... };
                std::sort(toRemoveIDs.begin(), toRemoveIDs.end());
                ArchetypeID archetypeID{ r.archetype->type };

                if (archetypeID.size() == toRemoveIDs.size()) {
                    return RemoveAll(entity);
                }

                for (std::size_t i{}; i != toRemoveIDs.size(); ++i) {
                    r.archetype->components[toRemoveIDs[i]]->Delete(GetIndex(GetType(entity)));
                    archetypeID.erase(std::find(archetypeID.begin(), archetypeID.end(), toRemoveIDs[i]));
                }

                Archetype* node{ FindArchetype(archetypeID) };
                PopEntity(entity, r.archetype);
                auto& components{ r.archetype->components };
                auto& type{ node->type };

                if (node->components.IsEmpty()) {
                    for (std::size_t i{}; i != type.size(); ++i) {
                        node->components.Add(type[i], components[type[i]]->Create());
                        components[type[i]]->Move(GetIndex(GetType(entity)) - 1, node->components[type[i]].get());
                    }
                } else {
                    for (std::size_t i{}; i != type.size(); ++i) {
                        components[type[i]]->Move(GetIndex(GetType(entity)) - 1, node->components[type[i]].get());
                    }
                }

                r.archetype = node;
                r.archetype->entities.emplace_back(entity);
            }

            void RemoveAll(Entity entity) noexcept {
                Record& r{ m_EntityArchetype[entity] };
                if (!r.archetype) {
                    return;
                }
                for (std::size_t i{}; i != r.archetype->components.GetSize(); ++i) {
                    r.archetype->components[r.archetype->type[i]]->Delete(GetIndex(GetType(entity)) - 1);
                }
                PopEntity(entity, r.archetype);
                r.archetype = nullptr;
            }

            template <typename... T>
            bool Contains(Entity entity) {
                Record& r{ m_EntityArchetype[entity] };
                if (!((Contains2<T>(entity)), ...)) {
                    return false;
                }
                return true;
            }

            template <typename... T>
            decltype(auto) Get(Entity entity) {
                Record& r{ m_EntityArchetype[entity] };
                ADH_THROW(Contains<T...>(entity), "Entity doesn't have component!");
                return std::forward_as_tuple(GetPointer<T>(r.archetype)->Get(GetIndex(GetType(entity)) - 1)...);
            }

            void Destroy(Entity entity) {
                if (IsValid(entity)) {
                    RemoveAll(entity);
                    DeleteID(entity);
                }
            }

            void Destroy() noexcept {
                Clear();
            }

            bool IsValid(Entity entity) const noexcept {
                std::uint32_t pos{ GetIndex(GetType(entity)) };
                return (pos <= m_Entities.GetSize() && m_Entities[pos - 1u] == entity);
            }

            template <typename... T>
            auto GetSystem() {
                System<T...> system;
                ArchetypeID ids{ GetID<T>()... };
                for (std::size_t i{}; i != m_Archetypes.GetSize(); ++i) {
                    if (!m_Archetypes[i]->entities.empty()) {
                        bool toEmp{ true };
                        for (std::size_t j{}; j != ids.size(); ++j) {
                            auto& type{ m_Archetypes[i]->type };
                            if (std::find(type.begin(), type.end(), ids[j]) == type.end()) {
                                toEmp = false;
                                break;
                            }
                        }
                        if (toEmp) {
                            system.archetypes.emplace_back(m_Archetypes[i]);
                        }
                    }
                }
                return system;
            }

            template <typename T>
            void ForEach(T func) {
                for (std::int64_t i = m_Entities.GetSize() - 1; i >= 0; --i) {
                    if (IsValid(m_Entities[i])) {
                        func(m_Entities[i]);
                    }
                }
            }

            std::size_t GetEntityCount() const noexcept {
                return m_Entities.GetSize() - m_RecicledEntities.GetSize();
            }

          private:
            void MoveConstruct(World&& rhs) noexcept {
                m_Archetypes        = std::move(rhs.m_Archetypes);
                m_EntityArchetype   = std::move(rhs.m_EntityArchetype);
                m_Entities          = std::move(rhs.m_Entities);
                m_RecicledEntities  = std::move(rhs.m_RecicledEntities);
                m_RootArchetype     = rhs.m_RootArchetype;
                rhs.m_RootArchetype = nullptr;
            }

            void Clear() noexcept {
                for (auto&& i : m_Archetypes) {
                    if (i) {
                        delete i;
                    }
                }
                if (m_RootArchetype) {
                    delete m_RootArchetype;
                }
                m_Archetypes       = {};
                m_EntityArchetype  = {};
                m_Entities         = {};
                m_RecicledEntities = {};
            }

            Entity GenerateID() {
                Entity id{ CreateID(m_Entities.GetSize() + 1, 0u) };
                m_Entities.EmplaceBack(id);
                m_EntityArchetype.emplace(id, Record{ nullptr });
                return id;
            }

            Entity RecycleID() {
                Entity temp{ m_RecicledEntities.Front() };
                m_RecicledEntities.Pop();
                Entity id                                = CreateID(GetIndex(GetType(temp)), GetVersion(GetType(temp)) + 1u);
                m_Entities[GetIndex(GetType(temp)) - 1u] = id;
                m_EntityArchetype.emplace(id, Record{ nullptr });
                return id;
            }

            Entity CreateID(std::uint32_t index, std::uint32_t version) const noexcept {
                return static_cast<Entity>((static_cast<EntityID>(index) << entity_shift) |
                                           (static_cast<std::uint32_t>(version)));
            }

            void DeleteID(Entity entity) {
                m_RecicledEntities.Emplace(entity);
                m_Entities[GetIndex(GetType(entity)) - 1u] = null_entity;
                m_EntityArchetype.erase(entity);
            }

            EntityID GetType(Entity entity) const noexcept {
                return static_cast<EntityID>(entity);
            }

            std::uint32_t GetIndex(EntityID entity) const noexcept {
                return static_cast<std::uint32_t>(entity >> entity_shift);
            }

            std::uint32_t GetVersion(EntityID entity) const noexcept {
                return static_cast<std::uint32_t>(entity);
            }

            template <typename... T>
            decltype(auto) GetArchetypeID(Archetype* node) {
                ArchetypeID archetypeID{ GetID<T>()... };
                if (node) {
                    for (std::size_t i{}; i != node->type.size(); ++i) {
                        archetypeID.emplace_back(node->type[i]);
                    }
                }
                std::sort(archetypeID.begin(), archetypeID.end());
                return archetypeID;
            }

            Archetype* FindArchetype(const ArchetypeID& archetypeID) {
                Archetype* node{ m_RootArchetype };
                for (std::size_t i{}; i != archetypeID.size(); ++i) {
                    Edge& edge{ node->edges[archetypeID[i]] };
                    if (!edge.next) {
                        Archetype* newArchetype{ new Archetype };
                        newArchetype->edges.emplace(archetypeID[i], Edge{ nullptr, node });
                        for (std::size_t j{}; j <= i; ++j) {
                            newArchetype->type.emplace_back(archetypeID[j]);
                        }
                        edge.next = newArchetype;
                        m_Archetypes.EmplaceBack(edge.next);
                    }
                    node = edge.next;
                }
                return node;
            }

            template <typename T>
            bool Contains2(Entity entity) noexcept {
                Record& r{ m_EntityArchetype[entity] };
                if (r.archetype) {
                    if (r.archetype->components.Contains(GetID<T>())) {
                        auto& entities{ r.archetype->entities };
                        if (std::find(entities.begin(), entities.end(), entity) != entities.end()) {
                            return true;
                        }
                    }
                }
                return false;
            }

            void PopEntity(Entity entity, Archetype* node) noexcept {
                auto itr{ std::find(node->entities.begin(), node->entities.end(), entity) };
                if (itr != node->entities.end()) {
                    *itr = node->entities[node->entities.size() - 1u];
                    node->entities.pop_back();
                }
            }

            template <typename T>
            void AddComponent(Archetype* node) {
                node->components.Add(GetID<T>(), new Container<T>());
            }

            template <typename T, typename... Args>
            void EmplaceData(Archetype* node, std::uint32_t index, Args&&... args) {
                GetPointer<T>(node)->Emplace(index, std::forward<Args>(args)...);
            }

            template <typename T>
            auto GetPointer(Archetype* node) noexcept {
                return static_cast<Container<T>*>(node->components[GetID<T>()].get());
            }

            template <typename T>
            ComponentID GetID() const noexcept {
                return TypeIDGenerator<ComponentID>::GetID<T>();
            }

          private:
            Array<Archetype*> m_Archetypes;
            std::unordered_map<Entity, Record> m_EntityArchetype;
            Array<Entity> m_Entities;
            Queue<Entity> m_RecicledEntities;
            Archetype* m_RootArchetype;
        };
    } // namespace ecs
} // namespace adh
