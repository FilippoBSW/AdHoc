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
    template <std::size_t N, std::size_t N2, typename T>
    auto operator+(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return Add(lhs, rhs);
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto& operator+=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return lhs = lhs + rhs;
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto operator-(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return Subtract(lhs, rhs);
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto& operator-=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return lhs = lhs - rhs;
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto operator*(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto& operator*=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return lhs = lhs * rhs;
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto operator*(const Matrix<N, N2, T>& lhs, T rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto& operator*=(Matrix<N, N2, T>& lhs, T rhs) noexcept {
        return lhs = lhs * rhs;
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto operator*(T lhs, const Matrix<N, N2, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <std::size_t N, std::size_t N2, typename T>
    auto Transpose(const Matrix<N, N2, T>& mat) noexcept {
        Matrix<N, N2, T> returnValue;

        for (std::size_t i{}; i != N; ++i) {
            for (std::size_t j{}; j != N2; ++j) {
                returnValue[j][i] = mat[i][j];
            }
        }

        return returnValue;
    }
} // namespace adh
