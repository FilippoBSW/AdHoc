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

namespace adh {
    template <typename T>
    class Vector<2u, T> {
      public:
        using Type = T;

      public:
        inline Vector<2u, T>& Add(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Subtract(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Multiply(T rhs) noexcept;

        inline Vector<2u, T>& Multiply(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Multiply(const Vector<2u, T>& rhs) noexcept;

        inline T Dot(const Vector<2u, T>& rhs) const noexcept;

        inline Vector<2u, T>& Divide(T rhs) noexcept;

        inline T Magnitude() const noexcept;

        inline Vector<2u, T>& Normalize() noexcept;

        inline Vector<2u, T>& Negate() noexcept;

        inline Vector<2u, T>& Rotate(T angle) noexcept;

        inline Vector<2u, T>& Reflect(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Refract(const Vector<2u, T>& rhs, T n) noexcept;

        inline std::size_t GetSize() const noexcept;

        inline T& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const T& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        union {
            struct {
                T x;
                T y;
            };

            T v[2u];
        };
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline Vector<2u, T> Add(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Subtract(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(T lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Divide(const Vector<2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline T Dot(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Rotate(const Vector<2u, T>& vec, T angle) noexcept;
} // namespace adh
