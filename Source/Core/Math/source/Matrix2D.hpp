#pragma once
#include "Vector2D.hpp"

namespace adh {
    template <typename T>
    class Matrix<2u, 2u, T> {
      public:
        using Type = T;

      public:
        inline Matrix() noexcept;

        inline Matrix(T identityValue) noexcept;

        template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1u)>>
        inline Matrix(Args&&... args) noexcept;

        inline auto& Add(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Subtract(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Multiply(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline auto& Multiply(T rhs) noexcept;

        inline void Identity() noexcept;

        inline auto& Transpose() noexcept;

        inline auto& Inverse() noexcept;

        inline auto& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const auto& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        Vector2D m[2];
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline auto Add(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Subtract(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<2u, 2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline auto Multiply(T lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline auto Inverse(const Matrix<2u, 2u, T>& mat) noexcept;
} // namespace adh
