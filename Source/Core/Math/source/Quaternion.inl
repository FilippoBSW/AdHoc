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
    Quaternion<T>::Quaternion() noexcept : v{} {
    }

    template <typename T>
    Quaternion<T>::Quaternion(T x, T y, T z, T w) noexcept : v{ x, y, z, w } {
    }

    template <typename T>
    template <std::size_t N>
    Quaternion<T>::Quaternion(T angle, const Vector<N, T>& axis) noexcept {
        Init(angle, axis);
    }

    template <typename T>
    template <std::size_t N>
    Quaternion<T>::Quaternion(const Vector<N, T>& angle) noexcept {
        Init(angle);
    }

    template <typename T>
    template <std::size_t N, std::size_t N2>
    Quaternion<T>::Quaternion(const Matrix<N, N2, T>& mat) noexcept {
        Init(mat);
    }

    template <typename T>
    inline Quaternion<T>::Quaternion(const xmm::Matrix& mat) noexcept {
        Init(mat);
    }

    template <typename T>
    template <std::size_t N>
    auto& Quaternion<T>::Reset(T angle, const Vector<N, T>& axis) noexcept {
        Init(angle, axis);

        return *this;
    }

    template <typename T>
    template <std::size_t N, std::size_t N2>
    auto& Quaternion<T>::Reset(const Matrix<N, N2, T>& mat) noexcept {
        Init(mat);

        return *this;
    }

    template <typename T>
    template <std::size_t N>
    auto Quaternion<T>::GetVector(const Vector<N, T>& vec) const noexcept {
        return Get(vec);
    }

    template <typename T>
    auto Quaternion<T>::GetVector(const xmm::Vector& vec) const noexcept {
        return Get(vec);
    }

    template <typename T>
    auto Quaternion<T>::GetMatrix3D() const noexcept {
        return Get3D();
    }

    template <typename T>
    auto Quaternion<T>::GetMatrix4D() const noexcept {
        return Get4D();
    }

    template <typename T>
    xmm::Matrix Quaternion<T>::GetXmmMatrix() const noexcept {
        return GetXmm();
    }

    template <typename T>
    auto& Quaternion<T>::operator[](std::size_t index) noexcept {
        return v[index];
    }

    template <typename T>
    const auto& Quaternion<T>::operator[](std::size_t index) const noexcept {
        return v[index];
    }

    template <typename T>
    template <std::size_t N>
    void Quaternion<T>::Init(T angle, const Vector<N, T>& axis) noexcept {
        Vector<3, T> temp{ Normalize(axis) * std::sin(angle / T(2)) };
        x = temp[0];
        y = temp[1];
        z = temp[2];
        w = std::cos(angle / T(2));
    }

    template <typename T>
    template <std::size_t N>
    void Quaternion<T>::Init(const Vector<N, T>& angle) noexcept {
        Vector<N, T> c;
        Vector<N, T> s;

        c[0] = std::cos(angle[0] * T(0.5));
        c[1] = std::cos(angle[1] * T(0.5));
        c[2] = std::cos(angle[2] * T(0.5));

        s[0] = std::sin(angle[0] * T(0.5));
        s[1] = std::sin(angle[1] * T(0.5));
        s[2] = std::sin(angle[2] * T(0.5));

        // x = s.x * c.y * c.z - c.x * s.y * s.z;
        // y = c.x * s.y * c.z + s.x * c.y * s.z;
        // z = c.x * c.y * s.z - s.x * s.y * c.z;
        // w = c.x * c.y * c.z + s.x * s.y * s.z;

        x = s.x * c.y * c.z + c.x * s.y * s.z;
        y = c.x * s.y * c.z - s.x * c.y * s.z;
        z = c.x * c.y * s.z - s.x * s.y * c.z;
        w = c.x * c.y * c.z + s.x * s.y * s.z;
    }

    template <typename T>
    template <std::size_t N, std::size_t N2>
    void Quaternion<T>::Init(const Matrix<N, N2, T>& mat) noexcept {
        if (mat[0][0] + mat[1][1] + mat[2][2] > T(0)) {
            w = std::sqrt(mat[0][0] + mat[1][1] + mat[2][2] + T(1)) * T(0.5);
            x = (mat[2][1] - mat[1][2]) * (T(0.25) / w);
            y = (mat[0][2] - mat[2][0]) * (T(0.25) / w);
            z = (mat[1][0] - mat[0][1]) * (T(0.25) / w);
        }

        else if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
            x = std::sqrt(mat[0][0] - mat[1][1] - mat[2][2] + T(1)) * T(0.5);
            y = (mat[1][0] + mat[0][1]) * (T(0.25) / x);
            z = (mat[0][2] + mat[2][0]) * (T(0.25) / x);
            w = (mat[2][1] - mat[1][2]) * (T(0.25) / x);
        }

        else if (mat[1][1] > mat[2][2]) {
            y = std::sqrt(mat[1][1] - mat[0][0] - mat[2][2] + T(1)) * T(0.5);
            x = (mat[1][0] + mat[0][1]) * (T(0.25) / y);
            z = (mat[2][1] + mat[1][2]) * (T(0.25) / y);
            w = (mat[0][2] - mat[2][0]) * (T(0.25) / y);
        }

        else {
            z = std::sqrt(mat[2][2] - mat[0][0] - mat[1][1] + T(1)) * T(0.5);
            x = (mat[0][2] + mat[2][0]) * (T(0.25) / z);
            y = (mat[2][1] + mat[1][2]) * (T(0.25) / z);
            w = (mat[1][0] - mat[0][1]) * (T(0.25) / z);
        }
    }

    template <typename T>
    void Quaternion<T>::Init(const xmm::Matrix& mat) noexcept {
        if (mat.f[0][0] + mat.f[1][1] + mat.f[2][2] > T(0)) {
            w = std::sqrt(mat.f[0][0] + mat.f[1][1] + mat.f[2][2] + T(1)) * T(0.5);
            x = (mat.f[2][1] - mat.f[1][2]) * (T(0.25) / w);
            y = (mat.f[0][2] - mat.f[2][0]) * (T(0.25) / w);
            z = (mat.f[1][0] - mat.f[0][1]) * (T(0.25) / w);
        }

        else if (mat.f[0][0] > mat.f[1][1] && mat.f[0][0] > mat.f[2][2]) {
            x = std::sqrt(mat.f[0][0] - mat.f[1][1] - mat.f[2][2] + T(1)) * T(0.5);
            y = (mat.f[1][0] + mat.f[0][1]) * (T(0.25) / x);
            z = (mat.f[0][2] + mat.f[2][0]) * (T(0.25) / x);
            w = (mat.f[2][1] - mat.f[1][2]) * (T(0.25) / x);
        }

        else if (mat.f[1][1] > mat.f[2][2]) {
            y = std::sqrt(mat.f[1][1] - mat.f[0][0] - mat.f[2][2] + T(1)) * T(0.5);
            x = (mat.f[1][0] + mat.f[0][1]) * (T(0.25) / y);
            z = (mat.f[2][1] + mat.f[1][2]) * (T(0.25) / y);
            w = (mat.f[0][2] - mat.f[2][0]) * (T(0.25) / y);
        }

        else {
            z = std::sqrt(mat.f[2][2] - mat.f[0][0] - mat.f[1][1] + T(1)) * T(0.5);
            x = (mat.f[0][2] + mat.f[2][0]) * (T(0.25) / z);
            y = (mat.f[2][1] + mat.f[1][2]) * (T(0.25) / z);
            w = (mat.f[1][0] - mat.f[0][1]) * (T(0.25) / z);
        }
    }

    template <typename T>
    auto Quaternion<T>::Get(const Vector<3, T>& vec) const noexcept {
        Vector<3, T> temp{ x, y, z };

        return Vector<3, T>{
            (vec * (w * w - (x * x + y * y + z * z))) +
            (temp * (Dot(vec, temp) * T(2))) +
            (Cross(vec, temp) * (w * T(2)))
        };
    }

    template <typename T>
    auto Quaternion<T>::Get(const Vector<4, T>& vec) const noexcept {
        Vector<3, T> lhs{ x, y, z };
        Vector<3, T> rhs{ vec.x, vec.y, vec.z };

        Vector<3, T> vec3D{
            (rhs * (w * w - (x * x + y * y + z * z))) +
            (lhs * (Dot(rhs, lhs) * T(2))) +
            (Cross(rhs, lhs) * (w * T(2)))
        };

        return Vector<4, T>{ vec3D[0], vec3D[1], vec3D[2], vec[3] };
    }

    template <typename T>
    inline xmm::Vector Quaternion<T>::Get(const xmm::Vector& vec) const noexcept {
        Vector<3, T> lhs{ x, y, z };
        Vector<3, T> rhs{ vec[0], vec[1], vec[2] };

        Vector<3, T> vec3D{
            (rhs * (w * w - (x * x + y * y + z * z))) +
            (lhs * (Dot(rhs, lhs) * T(2))) +
            (Cross(rhs, lhs) * (w * T(2)))
        };

        return xmm::Vector{ vec3D[0], vec3D[1], vec3D[2], vec[3] };
    }

    template <typename T>
    auto Quaternion<T>::Get3D() const noexcept {
        Matrix<3, 3u, T> returnValue;
        returnValue[0][0] = T(1) - T(2) * ((y * y) + (z * z));
        returnValue[0][1] = T(2) * ((x * y) + (w * z));
        returnValue[0][2] = T(2) * ((x * z) - (w * y));

        returnValue[1][0] = T(2) * ((x * y) - (w * z));
        returnValue[1][1] = T(1) - T(2) * ((x * x) + (z * z));
        returnValue[1][2] = T(2) * ((y * z) + (w * x));

        returnValue[2][0] = T(2) * ((x * z) + (w * y));
        returnValue[2][1] = T(2) * ((y * z) - (w * x));
        returnValue[2][2] = T(1) - T(2) * ((x * x) + (y * y));

        return returnValue;
    }

    template <typename T>
    auto Quaternion<T>::Get4D() const noexcept {
        Matrix<4u, 4u, T> returnValue;
        returnValue[0][0] = T(1) - T(2) * ((y * y) + (z * z));
        returnValue[0][1] = T(2) * ((x * y) + (w * z));
        returnValue[0][2] = T(2) * ((x * z) - (w * y));
        returnValue[0][3] = T(0);

        returnValue[1][0] = T(2) * ((x * y) - (w * z));
        returnValue[1][1] = T(1) - T(2) * ((x * x) + (z * z));
        returnValue[1][2] = T(2) * ((y * z) + (w * x));
        returnValue[1][3] = T(0);

        returnValue[2][0] = T(2) * ((x * z) + (w * y));
        returnValue[2][1] = T(2) * ((y * z) - (w * x));
        returnValue[2][2] = T(1) - T(2) * ((x * x) + (y * y));
        returnValue[2][3] = T(0);

        returnValue[3][0] = T(0);
        returnValue[3][1] = T(0);
        returnValue[3][2] = T(0);
        returnValue[3][3] = T(1);

        return returnValue;
    }

    template <typename T>
    inline xmm::Matrix Quaternion<T>::GetXmm() const noexcept {
        xmm::Matrix returnValue;
        returnValue[0][0] = T(1) - T(2) * ((y * y) + (z * z));
        returnValue[0][1] = T(2) * ((x * y) + (w * z));
        returnValue[0][2] = T(2) * ((x * z) - (w * y));
        returnValue[0][3] = T(0);

        returnValue[1][0] = T(2) * ((x * y) - (w * z));
        returnValue[1][1] = T(1) - T(2) * ((x * x) + (z * z));
        returnValue[1][2] = T(2) * ((y * z) + (w * x));
        returnValue[1][3] = T(0);

        returnValue[2][0] = T(2) * ((x * z) + (w * y));
        returnValue[2][1] = T(2) * ((y * z) - (w * x));
        returnValue[2][2] = T(1) - T(2) * ((x * x) + (y * y));
        returnValue[2][3] = T(0);

        returnValue[3][0] = T(0);
        returnValue[3][1] = T(0);
        returnValue[3][2] = T(0);
        returnValue[3][3] = T(1);

        return returnValue;
    }
} // namespace adh

namespace adh {
    template <typename T>
    auto operator*(const Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        return Quaternion<T>{
            lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
            lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
            lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
        };
    }

    template <typename T>
    auto& operator*=(Quaternion<T>& lhs, const Quaternion<T>& rhs) noexcept {
        return lhs = lhs * rhs;
    }
} // namespace adh
