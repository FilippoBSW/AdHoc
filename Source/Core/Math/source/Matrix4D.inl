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
#include <cmath>

namespace adh {
    template <typename T>
    Matrix<4u, 4u, T>::Matrix() noexcept : m{} {
    }

    template <typename T>
    Matrix<4u, 4u, T>::Matrix(T identityValue) noexcept : m{} {
        m[0].x = identityValue;
        m[1].y = identityValue;
        m[2].z = identityValue;
        m[3].w = identityValue;
    }

    template <typename T>
    template <typename... Args, typename>
    Matrix<4u, 4u, T>::Matrix(Args&&... args) noexcept : m{ std::forward<Args>(args)... } {
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Add(const Matrix<4u, 4u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Subtract(const Matrix<4u, 4u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Multiply(const Matrix<4u, 4u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    void Matrix<4u, 4u, T>::Identity() noexcept {
        *this = Matrix<4u, 4u, T>{ T(1) };
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Transpose() noexcept {
        return *this = adh::Transpose(*this);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Rotate(T angle, const Vector<3u, T>& axis) noexcept {
        return *this = adh::Rotate(*this, angle, axis);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Rotate(const Vector<3u, T>& angle) noexcept {
        return *this = adh::Rotate(*this, angle);
    }

    template <typename T>
    void Matrix<4u, 4u, T>::Decompose(Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept {
        adh::Decompose(*this, translation, rotation, scale);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Inverse() noexcept {
        return *this = adh::Inverse(*this);
    }

    template <typename T>
    Vector4D& Matrix<4u, 4u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 4u, "Vector out of range!");
        return m[index];
    }

    template <typename T>
    const Vector4D& Matrix<4u, 4u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 4u, "Vector out of range!");
        return m[index];
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Translate(const Vector<3u, T>& vec) noexcept {
        return *this = adh::Translate(*this, vec);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::Scale(const Vector<3u, T>& vec) noexcept {
        return *this = adh::Scale(*this, vec);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept {
        return *this = adh::LookAtLH(eyePos, focusPos, upVector);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept {
        return *this = adh::LookAtRH(eyePos, focusPos, upVector);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept {
        return *this = adh::OrthographicLH(left, right, bottom, top, nearZ, farZ);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept {
        return *this = adh::OrthographicRH(left, right, bottom, top, nearZ, farZ);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT {
        return *this = adh::PerspectiveLH(fovY, aspectRatio, nearZ, farZ);
    }

    template <typename T>
    auto& Matrix<4u, 4u, T>::PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT {
        return *this = adh::PerspectiveRH(fovY, aspectRatio, nearZ, farZ);
    }
} // namespace adh

namespace adh {
    template <typename T>
    auto Add(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept {
        return Matrix<4u, 4u, T>{
            lhs[0][0] + rhs[0][0], lhs[0][1] + rhs[0][1], lhs[0][2] + rhs[0][2], lhs[0][3] + rhs[0][3],
            lhs[1][0] + rhs[1][0], lhs[1][1] + rhs[1][1], lhs[1][2] + rhs[1][2], lhs[2][3] + rhs[1][3],
            lhs[2][0] + rhs[2][0], lhs[2][1] + rhs[2][1], lhs[2][2] + rhs[2][2], lhs[2][3] + rhs[2][3],
            lhs[3][0] + rhs[3][0], lhs[3][1] + rhs[3][1], lhs[3][2] + rhs[3][2], lhs[3][3] + rhs[3][3]
        };
    }

    template <typename T>
    auto Subtract(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept {
        return Matrix<4u, 4u, T>{
            lhs[0][0] - rhs[0][0], lhs[0][1] - rhs[0][1], lhs[0][2] - rhs[0][2], lhs[0][3] - rhs[0][3],
            lhs[1][0] - rhs[1][0], lhs[1][1] - rhs[1][1], lhs[1][2] - rhs[1][2], lhs[2][3] - rhs[1][3],
            lhs[2][0] - rhs[2][0], lhs[2][1] - rhs[2][1], lhs[2][2] - rhs[2][2], lhs[2][3] - rhs[2][3],
            lhs[3][0] - rhs[3][0], lhs[3][1] - rhs[3][1], lhs[3][2] - rhs[3][2], lhs[3][3] - rhs[3][3]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept {
        return Matrix<4u, 4u, T>{
            rhs[0][0] * lhs[0][0] + rhs[0][1] * lhs[1][0] + rhs[0][2] * lhs[2][0] + rhs[0][3] * lhs[3][0],
            rhs[0][0] * lhs[0][1] + rhs[0][1] * lhs[1][1] + rhs[0][2] * lhs[2][1] + rhs[0][3] * lhs[3][1],
            rhs[0][0] * lhs[0][2] + rhs[0][1] * lhs[1][2] + rhs[0][2] * lhs[2][2] + rhs[0][3] * lhs[3][2],
            rhs[0][0] * lhs[0][3] + rhs[0][1] * lhs[1][3] + rhs[0][2] * lhs[2][3] + rhs[0][3] * lhs[3][3],

            rhs[1][0] * lhs[0][0] + rhs[1][1] * lhs[1][0] + rhs[1][2] * lhs[2][0] + rhs[1][3] * lhs[3][0],
            rhs[1][0] * lhs[0][1] + rhs[1][1] * lhs[1][1] + rhs[1][2] * lhs[2][1] + rhs[1][3] * lhs[3][1],
            rhs[1][0] * lhs[0][2] + rhs[1][1] * lhs[1][2] + rhs[1][2] * lhs[2][2] + rhs[1][3] * lhs[3][2],
            rhs[1][0] * lhs[0][3] + rhs[1][1] * lhs[1][3] + rhs[1][2] * lhs[2][3] + rhs[1][3] * lhs[3][3],

            rhs[2][0] * lhs[0][0] + rhs[2][1] * lhs[1][0] + rhs[2][2] * lhs[2][0] + rhs[2][3] * lhs[3][0],
            rhs[2][0] * lhs[0][1] + rhs[2][1] * lhs[1][1] + rhs[2][2] * lhs[2][1] + rhs[2][3] * lhs[3][1],
            rhs[2][0] * lhs[0][2] + rhs[2][1] * lhs[1][2] + rhs[2][2] * lhs[2][2] + rhs[2][3] * lhs[3][2],
            rhs[2][0] * lhs[0][3] + rhs[2][1] * lhs[1][3] + rhs[2][2] * lhs[2][3] + rhs[2][3] * lhs[3][3],

            rhs[3][0] * lhs[0][0] + rhs[3][1] * lhs[1][0] + rhs[3][2] * lhs[2][0] + rhs[3][3] * lhs[3][0],
            rhs[3][0] * lhs[0][1] + rhs[3][1] * lhs[1][1] + rhs[3][2] * lhs[2][1] + rhs[3][3] * lhs[3][1],
            rhs[3][0] * lhs[0][2] + rhs[3][1] * lhs[1][2] + rhs[3][2] * lhs[2][2] + rhs[3][3] * lhs[3][2],
            rhs[3][0] * lhs[0][3] + rhs[3][1] * lhs[1][3] + rhs[3][2] * lhs[2][3] + rhs[3][3] * lhs[3][3]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<4u, 4u, T>& lhs, T rhs) noexcept {
        return Matrix<4u, 4u, T>{
            lhs[0][0] * rhs, lhs[0][1] * rhs, lhs[0][2] * rhs, lhs[0][3] * rhs,
            lhs[1][0] * rhs, lhs[1][1] * rhs, lhs[1][2] * rhs, lhs[1][3] * rhs,
            lhs[2][0] * rhs, lhs[2][1] * rhs, lhs[2][2] * rhs, lhs[2][3] * rhs,
            lhs[3][0] * rhs, lhs[3][1] * rhs, lhs[3][2] * rhs, lhs[3][3] * rhs
        };
    }

    template <typename T>
    auto Multiply(T lhs, const Matrix<4u, 4u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    auto Rotate(const Matrix<4u, 4u, T>& mat, T angle, const Vector<3u, T>& axis) noexcept {
        Quaternion q{ angle, axis };
        return mat * q.GetMatrix4D();
    }

    template <typename T>
    auto Rotate(const Matrix<4u, 4u, T>& mat, const Vector<3u, T>& angle) noexcept {
        Quaternion q{ angle };
        return mat * q.GetMatrix4D();
    }

    template <typename T>
    inline void Decompose(const Matrix<4u, 4u, T>& m, Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept {
        // Translation
        translation = Vector3D{ m[3][0], m[3][1], m[3][2] };

        Vector3D rows[3];
        for (std::int32_t i{}; i != 3; ++i) {
            for (std::int32_t j{}; j != 3; ++j) {
                rows[i][j] = m[i][j];
            }
        }

        // Scale
        scale.x = rows[0].Magnitude();
        rows[0].Scale(1.0f);
        scale.y = rows[1].Magnitude();
        rows[1].Scale(1.0f);
        scale.z = rows[2].Magnitude();
        rows[2].Scale(1.0f);

        // Rotation
        // rotation.y = std::asin(-rows[0][2]);
        // if (std::cos(rotation.y) != 0) {
        //     rotation.x = -std::atan2(rows[1][2], rows[2][2]);
        //     rotation.z = -std::atan2(rows[0][1], rows[0][0]);
        // } else {
        //     rotation.x = -std::atan2(-rows[2][0], rows[1][1]);
        //     rotation.z = 0;
        // }

        rotation.x = asinf(-rows[2][1]); // Pitch
        if (cosf(rotation.x) > 0.0001) {
            rotation.y = atan2f(rows[2][0], rows[2][2]); // Yaw
            rotation.z = atan2f(rows[0][1], rows[1][1]); // Roll
        } else {
            rotation.y = 0.0f;                            // Yaw
            rotation.z = atan2f(-rows[1][0], rows[0][0]); // Roll
        }
    }

    template <typename T>
    auto Translate(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Matrix<4u, 4u, T>{
            lhs[0],
            lhs[1],
            lhs[2],
            lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3]
        };
    }

    template <typename T>
    auto Scale(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Matrix<4u, 4u, T>{
            lhs[0] * rhs[0],
            lhs[1] * rhs[1],
            lhs[2] * rhs[2],
            lhs[3]
        };
    }

    template <typename T>
    auto LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept {
        const Vector<3u, T> forward{ Normalize(focusPos - eyePos) };
        const Vector<3u, T> right{ Normalize(Cross(forward, upVector)) };
        const Vector<3u, T> up{ Cross(right, forward) };

        return Matrix<4u, 4u, T>{
            -right[0],
            up[0],
            forward[0],
            T(0),
            -right[1],
            up[1],
            forward[1],
            T(0),
            -right[2],
            up[2],
            forward[2],
            T(0),
            Dot(right, eyePos),
            -Dot(up, eyePos),
            -Dot(forward, eyePos),
            T(1),
        };
    }

    template <typename T>
    auto LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept {
        const Vector<3u, T> forward{ Normalize(focusPos - eyePos) };
        const Vector<3u, T> right{ Normalize(Cross(forward, upVector)) };
        const Vector<3u, T> up{ Cross(right, forward) };

        return Matrix<4u, 4u, T>{
            right[0],
            up[0],
            -forward[0],
            T(0),
            right[1],
            up[1],
            -forward[1],
            T(0),
            right[2],
            up[2],
            -forward[2],
            T(0),
            -Dot(right, eyePos),
            -Dot(up, eyePos),
            Dot(forward, eyePos),
            T(1),
        };
    }

    template <typename T>
    auto OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept {
        Matrix<4u, 4u, T> returnValue{};
        returnValue[0][0] = T(2) / (right - left);
        returnValue[1][1] = T(2) / (top - bottom);

        if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
            returnValue[2][2] = T(1) / (farZ - nearZ);
            returnValue[3][0] = -(right + left) / (right - left);
            returnValue[3][1] = -(top + bottom) / (top - bottom);
            returnValue[3][2] = -nearZ / (farZ - nearZ);
        }

        else {
            returnValue[2][2] = T(2) / (farZ - nearZ);
            returnValue[3][0] = -(right + left) / (right - left);
            returnValue[3][1] = -(top + bottom) / (top - bottom);
            returnValue[3][2] = -(farZ + nearZ) / (farZ - nearZ);
        }

        returnValue[3][3] = T(1);

        return returnValue;
    }

    template <typename T>
    auto OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept {
        Matrix<4u, 4u, T> returnValue{};
        returnValue[0][0] = T(2) / (right - left);
        returnValue[1][1] = T(2) / (top - bottom);

        if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
            returnValue[2][2] = -T(1) / (farZ - nearZ);
            returnValue[3][0] = -(right + left) / (right - left);
            returnValue[3][1] = -(top + bottom) / (top - bottom);
            returnValue[3][2] = -nearZ / (farZ - nearZ);
        }

        else {
            returnValue[2][2] = -T(2) / (farZ - nearZ);
            returnValue[3][0] = -(right + left) / (right - left);
            returnValue[3][1] = -(top + bottom) / (top - bottom);
            returnValue[3][2] = -(farZ + nearZ) / (farZ - nearZ);
        }

        returnValue[3][3] = T(1);

        return returnValue;
    }

    template <typename T>
    auto PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT {
        ADH_THROW(nearZ > T(0), "NearZ needs to be bigger than zero!");
        const T tanFov{ std::sin(fovY / T(2)) / std::cos(fovY / T(2)) };

        Matrix<4u, 4u, T> returnValue{};
        returnValue[0][0] = T(1) / (aspectRatio * tanFov);
        returnValue[1][1] = T(1) / tanFov;
        returnValue[2][2] = -(farZ / (nearZ - farZ));
        returnValue[2][3] = T(1);

        if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
            returnValue[3][2] = -(farZ * nearZ) / (farZ - nearZ);
        }

        else {
            returnValue[3][2] = -(T(2) * farZ * nearZ) / (farZ - nearZ);
        }

        return returnValue;
    }

    template <typename T>
    auto PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT {
        ADH_THROW(nearZ > T(0), "NearZ needs to be bigger than zero!");
        const T tanFov{ std::sin(fovY / T(2)) / std::cos(fovY / T(2)) };

        Matrix<4u, 4u, T> returnValue{};
        returnValue[0][0] = T(1) / (aspectRatio * tanFov);
        returnValue[1][1] = T(1) / tanFov;
        returnValue[2][2] = farZ / (nearZ - farZ);
        returnValue[2][3] = T(-1);

        if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
            returnValue[3][2] = -(farZ * nearZ) / (farZ - nearZ);
        }

        else {
            returnValue[3][2] = -(T(2) * farZ * nearZ) / (farZ - nearZ);
        }

        return returnValue;
    }

    template <typename T>
    auto Inverse(const Matrix<4u, 4u, T>& mat) noexcept {
        const Vector<3u, T>& a{ reinterpret_cast<const Vector<3u, T>&>(mat[0]) };
        const Vector<3u, T>& b{ reinterpret_cast<const Vector<3u, T>&>(mat[1]) };
        const Vector<3u, T>& c{ reinterpret_cast<const Vector<3u, T>&>(mat[2]) };
        const Vector<3u, T>& d{ reinterpret_cast<const Vector<3u, T>&>(mat[3]) };

        const T& x{ mat[0][3] };
        const T& y{ mat[1][3] };
        const T& z{ mat[2][3] };
        const T& w{ mat[3][3] };

        Vector<3u, T> s{ Cross(a, b) };
        Vector<3u, T> t{ Cross(c, d) };
        Vector<3u, T> u{ a * y - b * x };
        Vector<3u, T> v{ c * w - d * z };

        T det{ T(1) / (Dot(s, v) + Dot(t, u)) };
        s *= det;
        t *= det;
        u *= det;
        v *= det;

        Vector<3u, T> r0 = Cross(b, v) + t * y;
        Vector<3u, T> r1 = Cross(v, a) - t * x;
        Vector<3u, T> r2 = Cross(d, u) + s * w;
        Vector<3u, T> r3 = Cross(u, c) - s * z;

        Matrix<4u, 4u, T> returnValue{
            r0.x, r1.x, r2.x, r3.x,
            r0.y, r1.y, r2.y, r3.y,
            r0.z, r1.z, r2.z, r3.z,
            -Dot(b, t), Dot(a, t), -Dot(d, s), Dot(c, s)
        };

        return returnValue;
    }
} // namespace adh
