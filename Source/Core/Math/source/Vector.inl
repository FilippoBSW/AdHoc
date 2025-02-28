

namespace adh {
    template <std::size_t N, typename T>
    auto operator+(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        return Add(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto& operator+=(Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        return lhs = lhs + rhs;
    }

    template <std::size_t N, typename T>
    auto operator-(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        return Subtract(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto operator-(Vector<N, T>& lhs) noexcept {
        return lhs = Negate(lhs);
    }

    template <std::size_t N, typename T>
    auto& operator-=(Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        return lhs = lhs - rhs;
    }

    template <std::size_t N, typename T>
    auto operator*(const Vector<N, T>& lhs, T rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto& operator*=(Vector<N, T>& lhs, T rhs) noexcept {
        return lhs = lhs * rhs;
    }

    template <std::size_t N, typename T>
    auto operator*(T lhs, const Vector<N, T>& rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto operator*(const Vector<N, T>& lhs, const Matrix<N, N, T>& rhs) noexcept {
        return Multiply(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto& operator*=(Vector<N, T>& lhs, const Matrix<N, N, T>& rhs) noexcept {
        return lhs = lhs * rhs;
    }

    template <std::size_t N, typename T>
    auto operator*(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        return Dot(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto operator^(const Vector<N, T>& lhs, const Vector<N, T>& rhs) ADH_NOEXCEPT {
        ADH_THROW(lhs.GetSize() > 2u, "Cannot perform cross product on 2D vector!");
        return Cross(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto operator/(const Vector<N, T>& lhs, T rhs) noexcept {
        return Divide(lhs, rhs);
    }

    template <std::size_t N, typename T>
    auto& operator/=(Vector<N, T>& lhs, T rhs) noexcept {
        return lhs = lhs / rhs;
    }

    template <std::size_t N, typename T>
    auto Magnitude(const Vector<N, T>& vec) noexcept {
        return std::sqrt(vec.Dot(vec));
    }

    template <std::size_t N, typename T>
    auto Normalize(const Vector<N, T>& vec) noexcept {
        return Divide(vec, Magnitude(vec));
    }

    template <std::size_t N, typename T>
    inline auto Scale(const Vector<N, T>& lhs, T rhs) noexcept {
        return lhs * rhs / lhs.Magnitude();
    }

    template <std::size_t N, typename T>
    auto Rotate(const Vector<N, T>& vec, T angle, const Vector<3u, T>& axis) noexcept {
        Quaternion q{ angle, axis };
        return q.GetVector(vec);
    }

    template <std::size_t N, typename T>
    auto Rotate(const Vector<N, T>& vec, const Vector<3u, T>& angle) noexcept {
        Quaternion q{ angle };
        return q.GetVector(vec);
    }

    template <std::size_t N, typename T>
    auto Reflect(const Vector<N, T>& lhs, const Vector<N, T>& rhs) noexcept {
        const Vector<N, T> v1{ lhs };
        const Vector<N, T> v2{ Normalize(rhs) };
        const T dot{ Dot(v2, v1) };

        return v1 - v2 * dot * T(2);
    }

    template <std::size_t N, typename T>
    auto Refract(const Vector<N, T>& lhs, const Vector<N, T>& rhs, T n) noexcept {
        const Vector<N, T> v1{ lhs };
        const Vector<N, T> v2{ Normalize(rhs) };
        const T dot{ Dot(v2, v1) };
        const T k{ T(1) - n * n * (T(1) - dot * dot) };

        return Vector<N, T>{ n * v1 - (n * dot + std::sqrt(k)) * v2 };
    }

    template <std::size_t N, typename T>
    auto Negate(const Vector<N, T>& vec) noexcept {
        return Multiply(vec, T(-1));
    }
} // namespace adh
