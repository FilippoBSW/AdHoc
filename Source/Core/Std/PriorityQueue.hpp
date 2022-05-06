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
#include "Algorithms.hpp"
#include "Array.hpp"
#include "Concepts.hpp"
#include "Iterator.hpp"
#include "Utility.hpp"
#include <Utility.hpp>

#include <iostream>

namespace adh {
    template <typename T, IsCallable Order = Greater<T>>
    class PriorityQueue {
      public:
        using Type          = T;
        using Pointer       = Type*;
        using Reference     = Type&;
        using CReference    = const Type&;
        using SizeType      = std::size_t;
        using Iterator      = Iterator<T>;
        using ConstIterator = const Iterator;
        using Container     = Array<Type>;

      public:
        PriorityQueue() : m_Data{} {
        }

        template <typename... Args, typename = std::enable_if_t<(!std::is_base_of<PriorityQueue, std::decay_t<Args>>::value && ...)>>
        PriorityQueue(Args&&... args) : m_Data{ Forward<Args>(args)... } {
            MakeHeap<Order>(m_Data.begin(), m_Data.end());
        }

        PriorityQueue(const Container& data) : m_Data{ data } {
            MakeHeap<Order>(m_Data.begin(), m_Data.end());
        }

        PriorityQueue(Container&& data) : m_Data{ Move(data) } {
            MakeHeap<Order>(m_Data.begin(), m_Data.end());
        }

        PriorityQueue(const PriorityQueue& rhs) {
            *this = rhs;
        }

        PriorityQueue& operator=(const PriorityQueue& rhs) {
            m_Data = rhs.m_Data;
        }

        PriorityQueue(PriorityQueue&& rhs) noexcept {
            *this = Move(rhs);
        }

        PriorityQueue& operator=(PriorityQueue&& rhs) noexcept {
            m_Data = Move(rhs.m_Data);
        }

        ~PriorityQueue() = default;

        Reference GetTop() noexcept {
            ADH_THROW(m_Data.GetSize() > 0, "Empty Queue!");
            return m_Data[0];
        }

        CReference GetTop() const noexcept {
            ADH_THROW(m_Data.GetSize() > 0, "Empty Queue!");
            return m_Data[0];
        }

        void Pop() noexcept {
            ADH_THROW(m_Data.GetSize() > 0, "Empty Queue!");
            m_Data[0] = Move(m_Data[GetSize() - 1u]);
            m_Data.PopBack();
            PercolateDown<Order, Container, SizeType>(m_Data, m_Data.GetSize(), 1u);
        }

        template <typename... Args>
        Reference Emplace(Args&&... args) {
            auto& temp{ m_Data.EmplaceBack(Forward<Args>(args)...) };
            PercolateUp<Order>(m_Data, m_Data.GetSize());
            return temp;
        }

        void IncreaseKey(SizeType index, const Type& newKey) {
            ADH_THROW(index < m_Data.GetSize(), "Index out of Range!");
            ADH_THROW(m_Data[index] < newKey, "New key is smaller!");
            m_Data[index] = newKey;
            PercolateUp<Order>(m_Data, index + 1u);
        }

        void IncreaseKey(SizeType index, Type&& newKey) {
            ADH_THROW(index < m_Data.GetSize(), "Index out of Range!");
            ADH_THROW(m_Data[index] < newKey, "New key is smaller!");
            m_Data[index] = Move(newKey);
            PercolateUp<Order>(m_Data, index + 1u);
        }

        void DecreaseKey(SizeType index, const Type& newKey) {
            ADH_THROW(index < m_Data.GetSize(), "Index out of Range!");
            ADH_THROW(m_Data[index] > newKey, "New key is bigger!");
            m_Data[index] = newKey;
            PercolateDown<Order, Container, SizeType>(m_Data, m_Data.GetSize(), index + 1u);
        }

        void DecreaseKey(SizeType index, Type&& newKey) {
            ADH_THROW(index < m_Data.GetSize(), "Index out of Range!");
            ADH_THROW(m_Data[index] > newKey, "New key is bigger!");
            m_Data[index] = Move(newKey);
            PercolateDown<Order, Container, SizeType>(m_Data, m_Data.GetSize(), index + 1u);
        }

        bool IsEmpty() const noexcept {
            return m_Data.GetSize() == 0u;
        }

        Pointer GetData() const noexcept {
            return m_Data.GetData();
        }

        SizeType GetSize() const noexcept {
            return m_Data.GetSize();
        }

        SizeType GetCapacity() const noexcept {
            return m_Data.GetCapacity();
        }

        Iterator begin() noexcept {
            return m_Data.begin();
        }

        Iterator end() noexcept {
            return m_Data.end();
        }

        ConstIterator begin() const noexcept {
            return m_Data.begin();
        }

        ConstIterator end() const noexcept {
            return m_Data.end();
        }

      private:
        Container m_Data;
    };
} // namespace adh
