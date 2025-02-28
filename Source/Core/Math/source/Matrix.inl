

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
