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
#include <Utility.hpp>

#include <iostream>

namespace adh {
    template <typename T>
    class Iterator {
      public:
        using Type          = T;
        using CType         = const T;
        using Reference     = T&;
        using CReference    = const T&;
        using Pointer       = T*;
        using CPointer      = const T*;
        using ConstIterator = const Iterator;
        using SizeType      = std::size_t;

      public:
        Iterator(Pointer ptr) : m_Data{ ptr } {
        }

        Iterator& operator++() noexcept {
            ++m_Data;
            return *this;
        }

        Iterator operator++(int) noexcept {
            Iterator temp{ *this };
            ++m_Data;
            return temp;
        }

        ConstIterator& operator++() const noexcept {
            ++m_Data;
            return *this;
        }

        ConstIterator operator++(int) const noexcept {
            ConstIterator temp{ *this };
            ++m_Data;
            return temp;
        }

        Iterator& operator--() noexcept {
            --m_Data;
            return *this;
        }

        Iterator operator--(int) noexcept {
            Iterator temp{ *this };
            --m_Data;
            return temp;
        }

        ConstIterator& operator--() const noexcept {
            --m_Data;
            return *this;
        }

        ConstIterator operator--(int) const noexcept {
            ConstIterator temp{ *this };
            --m_Data;
            return temp;
        }

        Reference operator*() noexcept {
            return *m_Data;
        }

        Pointer operator->() noexcept {
            return m_Data;
        }

        CReference operator*() const noexcept {
            return *m_Data;
        }

        CPointer operator->() const noexcept {
            return m_Data;
        }

        bool operator==(const Iterator& rhs) const {
            return m_Data == rhs.m_Data;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

        Iterator operator+(SizeType offset) noexcept {
            return m_Data + offset;
        }

        ConstIterator operator+(SizeType offset) const noexcept {
            return m_Data + offset;
        }

        Iterator operator-(SizeType offset) noexcept {
            return m_Data - offset;
        }

        ConstIterator operator-(SizeType offset) const noexcept {
            return m_Data - offset;
        }

        auto operator-(const Iterator& rhs) const noexcept {
            return m_Data - rhs.m_Data;
        }

      public:
        Pointer m_Data;
    };
} // namespace adh
