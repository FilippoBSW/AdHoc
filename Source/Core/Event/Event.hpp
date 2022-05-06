// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
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
#include <Event/EventTypes.hpp>
#include <Std/Array.hpp>
#include <Std/Function.hpp>
#include <Std/Queue.hpp>
#include <Std/SparseSet.hpp>
#include <Std/UniquePtr.hpp>
#include <Utility.hpp>

namespace adh {
    class Event {
      public:
        using Callbacks = UniquePtr<BaseSparseSet>;
        template <typename T>
        using Container = SparseSet<Function<void(T*)>, 256u>;
        template <typename T>
        using ContainerPtr = SparseSet<Function<void(T*)>, 256u>*;

      public:
        [[nodiscard]] static auto CreateListener() {
            return GetInstance().CreateListener2();
        }

        static void DestroyListener(EventListener& listener) noexcept {
            GetInstance().DestroyListener2(listener);
        }

        template <typename T, typename Function>
        static void AddListener(const EventListener& listener, Function callback) {
            ADH_THROW(GetInstance().IsAlive(listener), "Adding to dead listener!");
            GetInstance().AddListener2<T>(listener, callback);
        }

        template <typename T, typename Function, typename Class>
        static void AddListener(const EventListener& listener, Function callback, Class* instance) {
            ADH_THROW(GetInstance().IsAlive(listener), "Adding to dead listener!");
            GetInstance().AddListener2<T>(listener, callback, instance);
        }

        template <typename T>
        static void RemoveListener(EventListener& listener) {
            GetInstance().RemoveListener2<T>(listener);
        }

        template <typename T, typename... Args>
        static void Dispatch(Args&&... args) {
            GetInstance().Dispatch2<T>(Forward<Args>(args)...);
        }

      private:
        ADH_API static Event& GetInstance() noexcept;

        EventListener CreateListener2() {
            if (m_RecycledListeners.IsEmpty()) {
                return GenerateID();
            } else {
                return RecycleID();
            }
        }

        void DestroyListener2(EventListener& listener) noexcept {
            if (IsAlive(listener)) {
                RemoveAll(listener);
                DeleteID(listener);
            }
        }

        template <typename T, typename Function>
        void AddListener2(const EventListener& listener, Function callback) {
            ADH_THROW(IsAlive(listener), "Adding to dead listener!");
            GetPointer<T>()->Add(GetIndex(listener), callback);
        }

        template <typename T, typename Function, typename Class>
        void AddListener2(const EventListener& listener, Function callback, Class* instance) {
            ADH_THROW(IsAlive(listener), "Adding to dead listener!");
            GetInstance().GetPointer<T>()->Add(GetInstance().GetIndex(listener), [instance, callback](T* event) { (instance->*callback)(event); });
        }

        template <typename T>
        void RemoveListener2(EventListener& listener) {
            if (GetPointer<T>()->Contains(GetIndex(listener))) {
                GetPointer<T>()->Remove(GetIndex(listener));
            }
        }

        template <typename T, typename... Args>
        void Dispatch2(Args&&... args) {
            T event(Forward<Args>(args)...);
            auto& callbacks{ GetPointer<T>()->GetDense() };
            for (std::size_t i{}; i < callbacks.GetSize(); ++i) {
                if (!event.isHandled) {
                    callbacks[i](&event);
                } else {
                    break;
                }
            }
        }

        EventListener GenerateID() {
            return m_EventListeners.EmplaceBack(CreateID(static_cast<EventListenerIndex>(m_EventListeners.GetSize()), 0u));
        }

        EventListener RecycleID() noexcept {
            EventListener temp{ m_RecycledListeners.Front() };
            m_RecycledListeners.Pop();
            return CreateID(GetIndex(temp), GetVersion(temp) + 1u);
        }

        EventListener CreateID(EventListenerIndex index, EventListenerVersion version) noexcept {
            return static_cast<EventListener>((static_cast<EventListenerType>(index) << EventListenerShift) | (static_cast<EventListenerType>(version)));
        }

        void DeleteID(EventListener& listener) {
            m_RecycledListeners.Emplace(listener);
            listener = CreateID(EventListenerInvalid, GetVersion(listener) + 1u);
        }

        void RemoveAll(const EventListener& listener) noexcept {
            for (std::size_t i{}; i != m_Callbacks.GetSize(); ++i) {
                if (m_Callbacks[i]) {
                    if (m_Callbacks[i]->Contains(GetIndex(listener))) {
                        m_Callbacks[i]->Remove(GetIndex(listener));
                    }
                }
            }
        }

        EventListenerIndex GetIndex(const EventListener& listener) const noexcept {
            return static_cast<EventListenerIndex>(ToType(listener) >> EventListenerShift);
        }

        EventListenerVersion GetVersion(const EventListener& listener) const noexcept {
            return static_cast<EventListenerVersion>(listener);
        }

        EventListenerType ToType(const EventListener& listener) const noexcept {
            return static_cast<EventListenerType>(listener);
        }

        bool IsAlive(const EventListener& listener) const noexcept {
            return (ToType(listener) >> EventListenerShift) != EventListenerInvalid;
        }

        template <typename T>
        auto GetID() noexcept {
            static EventID eventId{ Register<T>() };
            return eventId;
        }

        template <typename T>
        auto Register() {
            m_Callbacks.EmplaceBack(MakeUnique<Container<T>>());
            return static_cast<EventID>(m_Callbacks.GetSize() - 1u);
        }

        template <typename T>
        auto GetPointer() noexcept {
            return static_cast<ContainerPtr<T>>(m_Callbacks[GetID<T>()].Get());
        }

      private:
        Array<Callbacks> m_Callbacks;
        Array<EventListener> m_EventListeners;
        Queue<EventListener> m_RecycledListeners;
    };
} // namespace adh
