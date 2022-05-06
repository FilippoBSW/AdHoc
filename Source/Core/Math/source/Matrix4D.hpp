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
#include "Vector4D.hpp"

namespace adh {
    template <typename T>
    class Matrix<4u, 4u, T> {
      public:
        using Type = T;

      public:
        inline Matrix() noexcept;

        inline Matrix(T identityValue) noexcept;

        template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1u)>>
        inline Matrix(Args&&... args) noexcept;

        inline auto& Add(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Subtract(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Multiply(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Multiply(T rhs) noexcept;

        inline void Identity() noexcept;

        inline auto& Transpose() noexcept;

        inline auto& Rotate(T angle, const Vector<3u, T>& axis) noexcept;

        inline auto& Rotate(const Vector<3u, T>& axis) noexcept;

        inline void Decompose(Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

        inline auto& Inverse() noexcept;

        inline Vector4D& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const Vector4D& operator[](std::size_t index) const ADH_NOEXCEPT;

        inline auto& Translate(const Vector<3u, T>& rhs) noexcept;

        inline auto& Scale(const Vector<3u, T>& rhs) noexcept;

        inline auto& LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

        inline auto& LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

        inline auto& OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

        inline auto& OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

        inline auto& PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

        inline auto& PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

      public:
        Vector4D m[4];
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline auto Add(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Subtract(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<4u, 4u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline auto Multiply(T lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Rotate(const Matrix<4u, 4u, T>& mat, T angle, const Vector<3u, T>& axis) noexcept;

    template <typename T>
    inline auto Rotate(const Matrix<4u, 4u, T>& mat, const Vector<3u, T>& angle) noexcept;

    template <typename T>
    inline void Decompose(const Matrix<4u, 4u, T>& m, Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

    template <typename T>
    inline auto Inverse(const Matrix<4u, 4u, T>& mat) noexcept;

    template <typename T>
    inline auto Translate(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline auto Scale(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline auto LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

    template <typename T>
    inline auto LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

    template <typename T>
    inline auto OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

    template <typename T>
    inline auto OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

    template <typename T>
    inline auto PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

    template <typename T>
    inline auto PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;
} // namespace adh
