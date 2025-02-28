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
