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
    Vector<4u, T>& Vector<4u, T>::Add(const Vector<4u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Subtract(const Vector<4u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Multiply(const Matrix<4u, 4u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Multiply(const Vector<4u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Divide(T rhs) noexcept {
        return *this = adh::Divide(*this, rhs);
    }

    template <typename T>
    T Vector<4u, T>::Dot(const Vector<4u, T>& rhs) const noexcept {
        return adh::Dot(*this, rhs);
    }

    template <typename T>
    Vector<4u, T> adh::Vector<4u, T>::Cross(const Vector<4u, T>& rhs) const noexcept {
        return adh::Cross(*this, rhs);
    }

    template <typename T>
    T Vector<4u, T>::Magnitude() const noexcept {
        return adh::Magnitude(*this);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Normalize() noexcept {
        return *this = adh::Divide(*this, adh::Magnitude(*this));
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Scale(T rhs) noexcept {
        return *this = adh::Scale(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Rotate(T angle, const Vector<3u, T>& axis) noexcept {
        return *this = adh::Rotate(*this, angle, axis);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Rotate(const Vector<3u, T>& angle) noexcept {
        return *this = adh::Rotate(*this, angle);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Reflect(const Vector<4u, T>& rhs) noexcept {
        return *this = adh::Reflect(*this, rhs);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Refract(const Vector<4u, T>& rhs, T n) noexcept {
        return *this = adh::Refract(*this, rhs, n);
    }

    template <typename T>
    Vector<4u, T>& Vector<4u, T>::Negate() noexcept {
        return *this = adh::Negate(*this);
    }

    template <typename T>
    std::size_t Vector<4u, T>::GetSize() const noexcept {
        return 4u;
    }

    template <typename T>
    T& Vector<4u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 4u, "Vector out of range!");
        return v[index];
    }

    template <typename T>
    const T& Vector<4u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 4u, "Vector out of range!");
        return v[index];
    }
} // namespace adh

namespace adh {
    template <typename T>
    Vector<4u, T> Add(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept {
        return Vector<4u, T>{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]};
    }

    template <typename T>
    Vector<4u, T> Subtract(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept {
        return Vector<4u, T>{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]};
    }

    template <typename T>
    Vector<4u, T> Multiply(const Vector<4u, T>& lhs, T rhs) noexcept {
        return Vector<4u, T>{lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs};
    }

    template <typename T>
    Vector<4u, T> Multiply(T lhs, const Vector<4u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    Vector<4u, T> Multiply(const Vector<4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept {
        return Vector<4u, T>{
            lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0] + lhs[2] * rhs[2][0] + lhs[3] * rhs[3][0],
            lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] + lhs[2] * rhs[2][1] + lhs[3] * rhs[3][1],
            lhs[0] * rhs[0][2] + lhs[1] * rhs[1][2] + lhs[2] * rhs[2][2] + lhs[3] * rhs[3][2],
            lhs[0] * rhs[0][3] + lhs[1] * rhs[1][3] + lhs[2] * rhs[2][3] + lhs[3] * rhs[3][3],
        };
    }

    template <typename T>
    Vector<4u, T> Multiply(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept {
        return Vector<4u, T>{lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3]};
    }

    template <typename T>
    Vector<4u, T> Divide(const Vector<4u, T>& lhs, T rhs) noexcept {
        auto d{(T(1) / rhs)};
        return Vector<4u, T>{lhs[0] * d, lhs[1] * d, lhs[2] * d, lhs[3] * d};
    }

    template <typename T>
    T Dot(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
    }

    template <typename T>
    Vector<4u, T> Cross(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept {
        return Vector<4u, T>{
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0],
            lhs[3]};
    }
} // namespace adh
