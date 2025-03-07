
#include <Utility.hpp>

namespace adh {
    template <typename T>
    Matrix<2u, 2u, T>::Matrix() noexcept : m{} {
    }

    template <typename T>
    Matrix<2u, 2u, T>::Matrix(T identityValue) noexcept : m{} {
        m[0].x = identityValue;
        m[1].y = identityValue;
    }

    template <typename T>
    template <typename... Args, typename>
    Matrix<2u, 2u, T>::Matrix(Args&&... args) noexcept : m{ std::forward<Args>(args)... } {
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Add(const Matrix<2u, 2u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Subtract(const Matrix<2u, 2u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Multiply(const Matrix<2u, 2u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    void Matrix<2u, 2u, T>::Identity() noexcept {
        *this = Matrix<2u, 2u, T>{ T(1) };
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Transpose() noexcept {
        return *this = adh::Transpose(*this);
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::Inverse() noexcept {
        return *this = adh::Inverse(*this);
    }

    template <typename T>
    auto& Matrix<2u, 2u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 2u, "Vector out of range!");
        return m[index];
    }

    template <typename T>
    const auto& Matrix<2u, 2u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 2u, "Vector out of range!");
        return m[index];
    }
} // namespace adh

namespace adh {
    template <typename T>
    auto Add(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Matrix<2u, 2u, T>{
            lhs[0][0] + rhs[0][0], lhs[0][1] + rhs[0][1],
            lhs[1][0] + rhs[1][0], lhs[1][1] + rhs[1][1]
        };
    }

    template <typename T>
    auto Subtract(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Matrix<2u, 2u, T>{
            lhs[0][0] - rhs[0][0], lhs[0][1] - rhs[0][1],
            lhs[1][0] - rhs[1][0], lhs[1][1] - rhs[1][1]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<2u, 2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Matrix<2u, 2u, T>{
            rhs[0][0] * lhs[0][0] + rhs[0][1] * lhs[1][0],
            rhs[0][0] * lhs[0][1] + rhs[0][1] * lhs[1][1],
            rhs[1][0] * lhs[0][0] + rhs[1][1] * lhs[1][0],
            rhs[1][0] * lhs[0][1] + rhs[1][1] * lhs[1][1]
        };
    }

    template <typename T>
    auto Multiply(const Matrix<2u, 2u, T>& lhs, T rhs) noexcept {
        return Matrix<2u, 2u, T>{
            lhs[0][0] * rhs, lhs[0][1] * rhs,
            lhs[1][0] * rhs, lhs[1][1] * rhs
        };
    }

    template <typename T>
    auto Multiply(T lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    auto operator*(T lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <typename T>
    auto Inverse(const Matrix<2u, 2u, T>& mat) noexcept {
        T det{ T(1) / (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) };

        return Matrix<2u, 2u, T>{
            mat[1][1] * det,
            -mat[0][1] * det,
            -mat[1][0] * det,
            mat[0][0] * det
        };
    }
} // namespace adh
