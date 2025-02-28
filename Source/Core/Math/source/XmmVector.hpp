#pragma once

namespace adh {
    namespace xmm {
        class Vector {
          public:
            inline Vector() noexcept;

            inline Vector(__m128 vec) noexcept;

            inline Vector(float rhs) noexcept;

            inline Vector(float x, float y, float z, float w) noexcept;

            inline Vector(const float* rhs) noexcept;

            inline Vector& Set(float rhs) noexcept;

            inline Vector& Set(float x, float y, float z, float w) noexcept;

            inline Vector& Load(const float* rhs) noexcept;

            inline Vector& Store(float* rhs) noexcept;

            inline Vector& Add(const Vector& rhs) noexcept;

            inline Vector& Subtract(const Vector& rhs) noexcept;

            inline Vector& Multiply(const Vector& rhs) noexcept;

            inline Vector& Multiply(float rhs) noexcept;

            inline Vector& Multiply(const Matrix& rhs) noexcept;

            inline Vector& Divide(const Vector& rhs) noexcept;

            inline Vector& Divide(float rhs) noexcept;

            inline float Dot(const Vector& rhs) const noexcept;

            inline Vector Cross(const Vector& rhs) const noexcept;

            inline float Magnitude() const noexcept;

            inline Vector& Normalize() noexcept;

            inline Vector& Negate() noexcept;

            inline Vector& Rotate(float angle, const Vector3D& axis) noexcept;

            inline Vector& Rotate(const Vector3D& angle) noexcept;

            inline Vector& Reflect(const Vector& rhs) noexcept;

            inline Vector& Refract(const Vector& rhs, float n) noexcept;

            inline float& operator[](std::size_t index) ADH_NOEXCEPT;

            inline const float& operator[](std::size_t index) const ADH_NOEXCEPT;

            inline operator __m128();

            inline operator const __m128() const;

          public:
            union {
                struct {
                    float x;
                    float y;
                    float z;
                    float w;
                };

                struct {
                    float r;
                    float g;
                    float b;
                    float a;
                };

                float f[4];
                __m128 v;
            };
        };
    } // namespace xmm
} // namespace adh

namespace adh {
    namespace xmm {
        inline Vector Set(float rhs) noexcept;

        inline Vector Set(float x, float y, float z, float w) noexcept;

        inline Vector Load(const float* rhs) noexcept;

        inline Vector Store(float* rhs) noexcept;

        inline Vector Add(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Subtract(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Multiply(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Multiply(const Vector& lhs, float rhs) noexcept;

        inline Vector Multiply(const Vector& lhs, const Matrix& rhs) noexcept;

        inline Vector Divide(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Divide(const Vector& lhs, float rhs) noexcept;

        inline float Dot(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Cross(const Vector& lhs, const Vector& rhs) noexcept;

        inline float Magnitude(const Vector& vec) noexcept;

        inline Vector Normalize(const Vector& vec) noexcept;

        inline Vector Rotate(const Vector& vec, float angle, const Vector3D& axis) noexcept;

        inline Vector Rotate(const Vector& vec, const Vector3D& angle) noexcept;

        inline Vector Negate(const Vector& vec) noexcept;

        inline Vector Reflect(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector Refract(const Vector& lhs, const Vector& rhs, float n) noexcept;

        inline Vector operator+(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector& operator+=(Vector& lhs, const Vector& rhs) noexcept;

        inline Vector operator-(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector operator-(const Vector& vec) noexcept;

        inline Vector& operator-=(Vector& lhs, const Vector& rhs) noexcept;

        inline Vector operator*(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector& operator*=(Vector& lhs, const Vector& rhs) noexcept;

        inline Vector operator*(const Vector& lhs, float rhs) noexcept;

        inline Vector operator*(float rhs, const Vector& lhs) noexcept;

        inline Vector operator*=(Vector& lhs, float rhs) noexcept;

        inline Vector operator*(const Vector& lhs, const Matrix& rhs) noexcept;

        inline Vector operator*=(Vector& lhs, const Matrix& rhs) noexcept;

        inline Vector operator/(const Vector& lhs, const Vector& rhs) noexcept;

        inline Vector& operator/=(Vector& lhs, const Vector& rhs) noexcept;
    } // namespace xmm
} // namespace adh
