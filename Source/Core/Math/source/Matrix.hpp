#pragma once

namespace adh {
    template <std::size_t N, std::size_t N2, typename T>
    inline auto operator+(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto& operator+=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto operator-(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto& operator-=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto operator*(const Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto& operator*=(Matrix<N, N2, T>& lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto operator*(const Matrix<N, N2, T>& lhs, T rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto& operator*=(Matrix<N, N2, T>& lhs, T rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto operator*(T lhs, const Matrix<N, N2, T>& rhs) noexcept;

    template <std::size_t N, std::size_t N2, typename T>
    inline auto Transpose(const Matrix<N, N2, T>& mat) noexcept;
} // namespace adh
