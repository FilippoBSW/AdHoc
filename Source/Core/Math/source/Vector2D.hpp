#pragma once

namespace adh {
    template <typename T>
    class Vector<2u, T> {
      public:
        using Type = T;

      public:
        inline Vector<2u, T>& Add(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Subtract(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Multiply(T rhs) noexcept;

        inline Vector<2u, T>& Multiply(const Matrix<2u, 2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Multiply(const Vector<2u, T>& rhs) noexcept;

        inline T Dot(const Vector<2u, T>& rhs) const noexcept;

        inline Vector<2u, T>& Divide(T rhs) noexcept;

        inline T Magnitude() const noexcept;

        inline Vector<2u, T>& Normalize() noexcept;

        inline Vector<2u, T>& Negate() noexcept;

        inline Vector<2u, T>& Rotate(T angle) noexcept;

        inline Vector<2u, T>& Reflect(const Vector<2u, T>& rhs) noexcept;

        inline Vector<2u, T>& Refract(const Vector<2u, T>& rhs, T n) noexcept;

        inline std::size_t GetSize() const noexcept;

        inline T& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const T& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        union {
            struct {
                T x;
                T y;
            };

            T v[2u];
        };
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline Vector<2u, T> Add(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Subtract(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(T lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Divide(const Vector<2u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline T Dot(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<2u, T> Rotate(const Vector<2u, T>& vec, T angle) noexcept;
} // namespace adh
