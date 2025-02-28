#pragma once
#include "Vector4D.hpp"

namespace adh {
    template <typename T>
    class Matrix<4u, 4u, T> {
      public:
        using Type = T;

      public:
        inline Matrix() noexcept;

        inline Matrix(T identityValue) noexcept;

        template <typename... Args, typename = std::enable_if_t<(sizeof...(Args) > 1u)>>
        inline Matrix(Args&&... args) noexcept;

        inline auto& Add(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Subtract(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Multiply(const Matrix<4u, 4u, T>& rhs) noexcept;

        inline auto& Multiply(T rhs) noexcept;

        inline void Identity() noexcept;

        inline auto& Transpose() noexcept;

        inline auto& Rotate(T angle, const Vector<3u, T>& axis) noexcept;

        inline auto& Rotate(const Vector<3u, T>& axis) noexcept;

        inline void Decompose(Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

        inline auto& Inverse() noexcept;

        inline Vector4D& operator[](std::size_t index) ADH_NOEXCEPT;

        inline const Vector4D& operator[](std::size_t index) const ADH_NOEXCEPT;

        inline auto& Translate(const Vector<3u, T>& rhs) noexcept;

        inline auto& Scale(const Vector<3u, T>& rhs) noexcept;

        inline auto& LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

        inline auto& LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

        inline auto& OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

        inline auto& OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

        inline auto& PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

        inline auto& PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

      public:
        Vector4D m[4];
    };
} // namespace adh

namespace adh {
    template <typename T>
    inline auto Add(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Subtract(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<4u, 4u, T>& lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Multiply(const Matrix<4u, 4u, T>& lhs, T rhs) noexcept;

    template <typename T>
    inline auto Multiply(T lhs, const Matrix<4u, 4u, T>& rhs) noexcept;

    template <typename T>
    inline auto Rotate(const Matrix<4u, 4u, T>& mat, T angle, const Vector<3u, T>& axis) noexcept;

    template <typename T>
    inline auto Rotate(const Matrix<4u, 4u, T>& mat, const Vector<3u, T>& angle) noexcept;

    template <typename T>
    inline void Decompose(const Matrix<4u, 4u, T>& m, Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept;

    template <typename T>
    inline auto Inverse(const Matrix<4u, 4u, T>& mat) noexcept;

    template <typename T>
    inline auto Translate(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline auto Scale(const Matrix<4u, 4u, T>& lhs, const Vector<3u, T>& rhs) noexcept;

    template <typename T>
    inline auto LookAtLH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

    template <typename T>
    inline auto LookAtRH(const Vector<3u, T>& eyePos, const Vector<3u, T>& focusPos, const Vector<3u, T>& upVector) noexcept;

    template <typename T>
    inline auto OrthographicLH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

    template <typename T>
    inline auto OrthographicRH(T left, T right, T bottom, T top, T nearZ, T farZ) noexcept;

    template <typename T>
    inline auto PerspectiveLH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;

    template <typename T>
    inline auto PerspectiveRH(T fovY, T aspectRatio, T nearZ, T farZ) ADH_NOEXCEPT;
} // namespace adh
