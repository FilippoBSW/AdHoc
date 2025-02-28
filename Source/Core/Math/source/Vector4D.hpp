#pragma once

namespace adh {
    template <typename T>
    class Vector<4u, T> {
      public:
        using Type = T;

      public:
        inline Vector<4u, T>& Add(const Vector<4u, T>& rhs) noexcept;

        inline Vector<4u, T>& Subtract(const Vector<4u, T>& rhs) noexcept;

        inline Vector<4u, T>& Multiply(T rhs) noexcept;

        inline Vector<4u, T>& Multiply(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline Vector<4u, T>& Multiply(const Vector<4u, T>& rhs) noexcept;

        inline Vector<4u, T>& Divide(T rhs) noexcept;

        inline T Dot(const Vector<4u, T>& rhs) const noexcept;

        inline Vector<4u, T> Cross(const Vector<4u, T>& rhs) const noexcept;

        inline T Magnitude() const noexcept;

        inline Vector<4u, T>& Normalize() noexcept;

        inline Vector<4u, T>& Scale(T rhs) noexcept;

        inline Vector<4u, T>& Rotate(T angle, const Vector<3u, T>& axis) noexcept;

        inline Vector<4u, T>& Rotate(const Vector<3u, T>& angle) noexcept;

        inline Vector<4u, T>& Reflect(const Vector<4u, T>& rhs) noexcept;

        inline Vector<4u, T>& Refract(const Vector<4u, T>& rhs, T n) noexcept;

        inline Vector<4u, T>& Negate() noexcept;

        inline std::size_t GetSize() const noexcept;

        inline T& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const T& operator[](std::size_t index) const ADH_NOEXCEPT;

      public:
        union {
            struct {
                T x;
                T y;
                T z;
                T w;
            };

            struct {
                T r;
                T g;
                T b;
                T a;
            };

            T v[4u];
        };
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline Vector<4u, T> Add(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Subtract(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Multiply(const Vector<4u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Multiply(T lhs, const Vector<4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Multiply(const Vector<4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Multiply(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Divide(const Vector<4u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Dot(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept;

    template <typename T>
    inline Vector<4u, T> Cross(const Vector<4u, T>& lhs, const Vector<4u, T>& rhs) noexcept;
} // namespace adh
