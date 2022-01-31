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
#include <cmath>
#include <numbers>

namespace adh {

    template <typename T>
    class Quaternion {
      public:
        using Type = T;

      public:
        inline Quaternion() noexcept;

        inline Quaternion(T x, T y, T z, T w) noexcept;

        template <std::size_t N>
        inline Quaternion(T angle, const Vector<N, T>& axis) noexcept;

        template <std::size_t N>
        inline Quaternion(const Vector<N, T>& angle) noexcept;

        template <std::size_t N, std::size_t N2>
        inline Quaternion(const Matrix<N, N2, T>& mat) noexcept;

        inline Quaternion(const xmm::Matrix& mat) noexcept;

        template <std::size_t N>
        inline auto& Reset(T angle, const Vector<N, T>& axis) noexcept;

        template <std::size_t N, std::size_t N2>
        inline auto& Reset(const Matrix<N, N2, T>& mat) noexcept;

        template <std::size_t N>
        inline auto GetVector(const Vector<N, T>& vec) const noexcept;

        inline auto GetVector(const xmm::Vector& vec) const noexcept;

        inline auto GetMatrix3D() const noexcept;

        inline auto GetMatrix4D() const noexcept;

        inline xmm::Matrix GetXmmMatrix() const noexcept;

        inline auto& operator[](std::size_t index) noexcept;

        inline const auto& operator[](std::size_t index) const noexcept;

      private:
        template <std::size_t N>
        inline void Init(T angle, const Vector<N, T>& axis) noexcept;

        template <std::size_t N>
        inline void Init(const Vector<N, T>& angle) noexcept;

        template <std::size_t N, std::size_t N2>
        inline void Init(const Matrix<N, N2, T>& mat) noexcept;

        inline void Init(const xmm::Matrix& mat) noexcept;

        inline auto Get(const Vector<3, T>& vec) const noexcept;

        inline auto Get(const Vector<4, T>& vec) const noexcept;

        inline xmm::Vector Get(const xmm::Vector& vec) const noexcept;

        inline auto Get3D() const noexcept;

        inline auto Get4D() const noexcept;

        inline xmm::Matrix GetXmm() const noexcept;

      public:
        union {
            struct
            {
                T x;
                T y;
                T z;
                T w;
            };

            Vector4D v;
        };
    };

    // from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    template <typename T>
    struct EulerAngles {
        T roll, pitch, yaw;
    };

    template <typename T>
    inline EulerAngles<T> ToEulerAngles(const adh::Quaternion<T>& q) {
        EulerAngles<T> angles;

        // roll (x-axis rotation)
        double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
        double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
        angles.roll      = std::atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        double sinp = 2 * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1)
            angles.pitch = std::copysign(std::numbers::pi / 2, sinp); // use 90 degrees if out of range
        else
            angles.pitch = std::asin(sinp);

        // yaw (z-axis rotation)
        double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
        double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
        angles.yaw       = std::atan2(siny_cosp, cosy_cosp);

        return angles;
    }
} // namespace adh

namespace adh {
    template <typename T>
    inline auto operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;

    template <typename T>
    inline auto& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept;
} // namespace adh
