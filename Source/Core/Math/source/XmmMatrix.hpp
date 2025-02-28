#pragma once
#include "XmmVector.hpp"

namespace adh {
    namespace xmm {
        class Matrix {
          public:
            inline Matrix() noexcept;

            inline Matrix(float identityValue) noexcept;

            template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1u)>>
            inline Matrix(Args&&... args) noexcept;

            inline auto& Add(const Matrix& rhs) noexcept;

            inline auto& Subtract(const Matrix& rhs) noexcept;

            inline auto& Multiply(const Matrix& rhs) noexcept;

            inline auto& Multiply(float rhs) noexcept;

            inline void Identity() noexcept;

            inline auto& Transpose() noexcept;

            inline auto& Inverse() noexcept;

            inline auto& Translate(const Vector3D& rhs) noexcept;

            inline auto& Scale(const Vector3D& rhs) noexcept;

            inline auto& Rotate(float angle, const Vector3D& axis) noexcept;

            inline auto& Rotate(const Vector3D& angle) noexcept;

            inline void Decompose(Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

            inline auto& LookAtLH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept;

            inline auto& LookAtRH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept;

            inline auto& OrthographicLH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;

            inline auto& OrthographicRH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;

            inline auto& PerspectiveLH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT;

            inline auto& PerspectiveRH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT;

            inline Vector& operator[](std::size_t index) noexcept;

            inline const Vector& operator[](std::size_t index) const noexcept;

          public:
            union {
                Vector m[4];
                float f[4][4];
            };
        };
    } // namespace xmm
} // namespace adh

namespace adh {
    namespace xmm {
        inline Matrix Add(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix Subtract(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix Multiply(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix Multiply(const Matrix& lhs, float rhs) noexcept;

        inline Matrix Transpose(const Matrix& lhs) noexcept;

        inline Matrix Inverse(const Matrix& mat) noexcept;

        inline Matrix Translate(const Matrix& lhs, const Vector3D& rhs) noexcept;

        inline Matrix Scale(const Matrix& lhs, const Vector3D& rhs) noexcept;

        inline Matrix Rotate(const Matrix& mat, float angle, const Vector3D& axis) noexcept;

        inline Matrix Rotate(const Matrix& mat, const Vector3D& angle) noexcept;

        inline void Decompose(const Matrix& m, Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

        inline Matrix LookAtLH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept;

        inline Matrix LookAtRH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept;

        inline Matrix OrthographicLH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;

        inline Matrix OrthographicRH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept;

        inline Matrix PerspectiveLH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT;

        inline Matrix PerspectiveRH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT;

        inline Matrix operator+(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix& operator+=(Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix operator-(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix& operator-=(Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix operator*(const Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix& operator*=(Matrix& lhs, const Matrix& rhs) noexcept;

        inline Matrix operator*(const Matrix& lhs, float rhs) noexcept;

        inline Matrix& operator*=(Matrix& lhs, float rhs) noexcept;
    } // namespace xmm
} // namespace adh
