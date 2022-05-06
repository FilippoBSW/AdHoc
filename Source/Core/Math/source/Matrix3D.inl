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
    Matrix<3u, 3u, T>::Matrix() noexcept : m{} {
    }

    template <typename T>
    Matrix<3u, 3u, T>::Matrix(T identityValue) noexcept : m{} {
        m[0].x = identityValue;
        m[1].y = identityValue;
        m[2].z = identityValue;
    }

    template <typename T>
    template <typename... Args, typename>
    Matrix<3u, 3u, T>::Matrix(Args&&... args) noexcept : m{ std::forward<Args>(args)... } {
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Add(const Matrix<3u, 3u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Subtract(const Matrix<3u, 3u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Multiply(const Matrix<3u, 3u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    void Matrix<3u, 3u, T>::Identity() noexcept {
        *this = Matrix<3u, 3u, T>{ T(1) };
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Transpose() noexcept {
        return *this = adh::Transpose(*this);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Rotate(T angle, const Vector<3u, T>& axis) noexcept {
        return *this = adh::Rotate(*this, angle, axis);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Rotate(const Vector<3u, T>& angle) noexcept {
        return *this = adh::Rotate(*this, angle);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::Inverse() noexcept {
        return *this = adh::Inverse(*this);
    }

    template <typename T>
    auto& Matrix<3u, 3u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 3u, "Vector out of range!");
        return m[index];
    }

    template <typename T>
    const auto& Matrix<3u, 3u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 3u, "Vector out of range!");
        return m[index];
    }
} // namespace adh

namespace adh {
    template <typename T>
    auto Add(const Matrix<3u, 3u, T>& lhs, const Matrix<3u, 3u, T>& rhs) noexcept {
        return Matrix<3u, 3u, T>{
            lhs[0][0] + rhs[0][0], lhs[0][1] + rhs[0][1], lhs[0][2] + rhs[0][2],
            lhs[1][0] + rhs[1][0], lhs[1][1] + rhs[1][1], lhs[1][2] + rhs[1][2],
            lhs[2][0] + rhs[2][0], lhs[2][1] + rhs[2][1], lhs[2][2] + rhs[2][2]
        };
    }

    template <typename T>
    auto Subtract(const Matrix<3u, 3u, T>& lhs, const Matrix<3u, 3u, T>& rhs) noexcept {
        return Matrix<3u, 3u, T>{
            lhs[0][0] - rhs[0][0], lhs[0][1] - rhs[0][1], lhs[0][2] - rhs[0][2],
            lhs[1][0] - rhs[1][0], lhs[1][1] - rhs[1][1], lhs[1][2] - rhs[1][2],
            lhs[2][0] - rhs[2][0], lhs[2][1] - rhs[2][1], lhs[2][2] - rhs[2][2]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<3u, 3u, T>& lhs, const Matrix<3u, 3u, T>& rhs) noexcept {
        return Matrix<3u, 3u, T>{
            rhs[0][0] * lhs[0][0] + rhs[0][1] * lhs[1][0] + rhs[0][2] * lhs[2][0],
            rhs[0][0] * lhs[0][1] + rhs[0][1] * lhs[1][1] + rhs[0][2] * lhs[2][1],
            rhs[0][0] * lhs[0][2] + rhs[0][1] * lhs[1][2] + rhs[0][2] * lhs[2][2],

            rhs[1][0] * lhs[0][0] + rhs[1][1] * lhs[1][0] + rhs[1][2] * lhs[2][0],
            rhs[1][0] * lhs[0][1] + rhs[1][1] * lhs[1][1] + rhs[1][2] * lhs[2][1],
            rhs[1][0] * lhs[0][2] + rhs[1][1] * lhs[1][2] + rhs[1][2] * lhs[2][2],

            rhs[2][0] * lhs[0][0] + rhs[2][1] * lhs[1][0] + rhs[2][2] * lhs[2][0],
            rhs[2][0] * lhs[0][1] + rhs[2][1] * lhs[1][1] + rhs[2][2] * lhs[2][1],
            rhs[2][0] * lhs[0][2] + rhs[2][1] * lhs[1][2] + rhs[2][2] * lhs[2][2]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<3u, 3u, T>& lhs, T rhs) noexcept {
        return Matrix<3u, 3u, T>{
            lhs[0][0] * rhs, lhs[0][1] * rhs, lhs[0][2] * rhs,
            lhs[1][0] * rhs, lhs[1][1] * rhs, lhs[1][2] * rhs,
            lhs[2][0] * rhs, lhs[2][1] * rhs, lhs[2][2] * rhs
        };
    }

    template <typename T>
    auto Multiply(T lhs, const Matrix<3u, 3u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    auto Rotate(const Matrix<3u, 3u, T>& mat, T angle, const Vector<3u, T>& axis) noexcept {
        Quaternion q{ angle, axis };
        return mat * q.GetMatrix3D();
    }

    template <typename T>
    auto Rotate(const Matrix<3u, 3u, T>& mat, const Vector<3u, T>& angle) noexcept {
        Quaternion q{ angle };
        return mat * q.GetMatrix3D();
    }

    template <typename T>
    auto Inverse(const Matrix<3u, 3u, T>& mat) noexcept {
        const Vector<3u, T>& a{ mat[0] };
        const Vector<3u, T>& b{ mat[1] };
        const Vector<3u, T>& c{ mat[2] };

        Vector<3u, T> r0 = Cross(c, b);
        Vector<3u, T> r1 = Cross(a, c);
        Vector<3u, T> r2 = Cross(b, a);

        T det{ T(1) / Dot(r2, c) };
        r0 *= det;
        r1 *= det;
        r2 *= det;

        Matrix<3u, 3u, T> returnValue{
            r0.x, r1.x, r2.x,
            r0.y, r1.y, r2.y,
            r0.z, r1.z, r2.z
        };

        return returnValue;
    }
} // namespace adh
