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

namespace adh {
    template <typename T>
    class Vector<3u, T> {
      public:
        using Type = T;

      public:
        inline Vector<3u, T>& Add(const Vector<3u, T>& rhs) noexcept;

        inline Vector<3u, T>& Subtract(const Vector<3u, T>& rhs) noexcept;

        inline Vector<3u, T>& Multiply(T rhs) noexcept;

        inline Vector<3u, T>& Multiply(const Matrix<3u, 3u, T>& rhs) noexcept;

        inline Vector<3u, T>& Multiply(const Vector<3u, T>& rhs) noexcept;

        inline Vector<3u, T>& Divide(T rhs) noexcept;

        inline T Dot(const Vector<3u, T>& rhs) const noexcept;

        inline Vector<3u, T> Cross(const Vector<3u, T>& rhs) const noexcept;

        inline T Magnitude() const noexcept;

        inline Vector<3u, T>& Normalize() noexcept;

        inline Vector<3u, T>& Rotate(T angle, const Vector<3u, T>& axis) noexcept;

        inline Vector<3u, T>& Rotate(const Vector<3u, T>& angle) noexcept;

        inline Vector<3u, T>& Reflect(const Vector<3u, T>& rhs) noexcept;

        inline Vector<3u, T>& Refract(const Vector<3u, T>& rhs, T n) noexcept;

        inline Vector<3u, T>& Negate() noexcept;

        inline Vector<3u, T>& Scale(T rhs) noexcept;

        inline std::size_t GetSize() const noexcept;

        inline T& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const T& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        union {
            struct {
                T x;
                T y;
                T z;
            };

            struct {
                T r;
                T g;
                T b;
            };

            T v[3u];
        };
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline Vector<3u, T> Add(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Subtract(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Multiply(const Vector<3u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Multiply(T lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Multiply(const Vector<3u, T>& lhs, const Matrix<3u, 3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Multiply(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Divide(const Vector<3u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline T Dot(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<3u, T> Cross(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept;
} // namespace adh
