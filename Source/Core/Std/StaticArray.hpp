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
#include "Concepts.hpp"
#include "Iterator.hpp"
#include "Utility.hpp"
#include <Utility.hpp>

#include <iostream>

namespace adh {
    template <typename T, std::size_t Size>
    class StaticArray {
      public:
        using Type          = T;
        using Pointer       = Type*;
        using CPointer      = const Type*;
        using Reference     = Type&;
        using CReference    = const Type&;
        using SizeType      = std::size_t;
        using Iterator      = Iterator<T>;
        using ConstIterator = const Iterator;

      public:
        Pointer GetData() noexcept {
            return m_Data;
        }

        CPointer GetData() const noexcept {
            return m_Data;
        }

        constexpr SizeType GetSize() const noexcept {
            return Size;
        }

        constexpr bool IsEmpty() const noexcept {
            return GetSize() == 0u;
        }

        void Fill(const Type& value) {
            if (sizeof(Type) == sizeof(char)) {
                std::memset(m_Data, static_cast<int>(value), GetSize());
            } else {
                for (SizeType i{}; i != GetSize(); ++i) {
                    m_Data[i] = value;
                }
            }
        }

        template <IsCallable Order = Lesser<Type>>
        void Sort() noexcept {
            return adh::Sort<Order>(begin(), end());
        }

        Reference operator[](SizeType index) noexcept {
            return m_Data[index];
        }

        constexpr CReference operator[](SizeType index) const noexcept {
            return m_Data[index];
        }

        Iterator begin() noexcept {
            return m_Data;
        }

        Iterator end() noexcept {
            return m_Data + GetSize();
        }

        ConstIterator begin() const noexcept {
            return m_Data;
        }

        ConstIterator end() const noexcept {
            return m_Data + GetSize();
        }

      public:
        Type m_Data[Size ? Size : 1u];
    };
} // namespace adh
