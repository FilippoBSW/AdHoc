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
    template <std::size_t N, typename T>
    inline auto operator+(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto& operator+=(Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator-(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator-(Vector<N, T>& lhs) noexcept;

    template <std::size_t N, typename T>
    inline auto& operator-=(Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator*(const Vector<N, T>& lhs, T rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator*(T lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto& operator*=(Vector<N, T>& lhs, T rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator*(const Vector<N, T>& lhs, const Matrix<N, N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto& operator*=(Vector<N, T>& lhs, const Matrix<N, N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator*(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto operator^(const Vector<N, T>& lhs, const Vector<N, T>& rhs) ADH_NOEXCEPT;

    template <std::size_t N, typename T>
    inline auto operator/(const Vector<N, T>& lhs, T rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto& operator/=(Vector<N, T>& lhs, T rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto Magnitude(const Vector<N, T>& vec) noexcept;

    template <std::size_t N, typename T>
    inline auto Normalize(const Vector<N, T>& vec) noexcept;

    template <std::size_t N, typename T>
    inline auto Scale(const Vector<N, T>& lhs, T rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto Rotate(const Vector<N, T>& vec, T angle, const Vector<3u, T>& axis) noexcept;

    template <std::size_t N, typename T>
    inline auto Rotate(const Vector<N, T>& vec, const Vector<3u, T>& angle) noexcept;

    template <std::size_t N, typename T>
    inline auto Reflect(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept;

    template <std::size_t N, typename T>
    inline auto Refract(const Vector<N, T>& lhs, const Vector<N, T>& rhs, T n) noexcept;

    template <std::size_t N, typename T>
    inline auto Negate(const Vector<N, T>& vec) noexcept;
} // namespace adh
