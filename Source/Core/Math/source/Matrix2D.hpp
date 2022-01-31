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
#include "Vector2D.hpp"

namespace adh {
    template <typename T>
    class Matrix<2u, 2u, T> {
      public:
        using Type = T;

      public:
        inline Matrix() noexcept;

        inline Matrix(T identityValue) noexcept;

        template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1u)>>
        inline Matrix(Args&&... args) noexcept;

        inline auto& Add(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Subtract(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Multiply(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Multiply(T rhs) noexcept;

        inline void Identity() noexcept;

        inline auto& Transpose() noexcept;

        inline auto& Inverse() noexcept;

        inline auto& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const auto& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        Vector2D m[2];
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline auto Add(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Subtract(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<2u, 2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline auto Multiply(T lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Inverse(const Matrix<2u, 2u, T>& mat) noexcept;
} // namespace adh
