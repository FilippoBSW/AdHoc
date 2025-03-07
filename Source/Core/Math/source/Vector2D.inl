

namespace adh {
    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Add(const Vector<2u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Subtract(const Vector<2u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Multiply(const Matrix<2u, 2u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Multiply(const Vector<2u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    T Vector<2u, T>::Dot(const Vector<2u, T>& rhs) const noexcept {
        return adh::Dot(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Divide(T rhs) noexcept {
        return *this = adh::Divide(*this, rhs);
    }

    template <typename T>
    T Vector<2u, T>::Magnitude() const noexcept {
        return adh::Magnitude(*this);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Normalize() noexcept {
        return *this = adh::Divide(*this, adh::Magnitude(*this));
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Negate() noexcept {
        return *this = adh::Negate(*this);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Rotate(T angle) noexcept {
        return *this = adh::Rotate(*this, angle);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Reflect(const Vector<2u, T>& rhs) noexcept {
        return *this = adh::Reflect(*this, rhs);
    }

    template <typename T>
    Vector<2u, T>& Vector<2u, T>::Refract(const Vector<2u, T>& rhs, T n) noexcept {
        return *this = adh::Refract(*this, rhs, n);
    }

    template <typename T>
    std::size_t Vector<2u, T>::GetSize() const noexcept {
        return 2u;
    }

    template <typename T>
    T& Vector<2u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 2u, "Vector out of range!");
        return v[index];
    }

    template <typename T>
    const T& Vector<2u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 2u, "Vector out of range!");
        return v[index];
    }
} // namespace adh

namespace adh {
    template <typename T>
    Vector<2u, T> Add(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept {
        return Vector<2u, T>{ lhs[0] + rhs[0], +lhs[1] + rhs[1] };
    }

    template <typename T>
    Vector<2u, T> Subtract(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept {
        return Vector<2u, T>{ lhs[0] - rhs[0], lhs[1] - rhs[1] };
    }

    template <typename T>
    Vector<2u, T> Multiply(const Vector<2u, T>& lhs, T rhs) noexcept {
        return Vector<2u, T>{ lhs[0] * rhs, lhs[1] * rhs };
    }

    template <typename T>
    Vector<2u, T> Multiply(T lhs, const Vector<2u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Matrix<2u, 2u, T>& rhs) noexcept {
        return Vector<2u, T>{
            lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0],
            lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1],
        };
    }

    template <typename T>
    Vector<2u, T> Multiply(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept {
        return Vector<2u, T>{ lhs[0] * rhs[0], lhs[1] * rhs[1] };
    }

    template <typename T>
    Vector<2u, T> Divide(const Vector<2u, T>& lhs, T rhs) noexcept {
        auto d{ (T(1) / rhs) };
        return Vector<2u, T>{ lhs[0] * d, lhs[1] * d };
    }

    template <typename T>
    Vector<2u, T> Dot(const Vector<2u, T>& lhs, const Vector<2u, T>& rhs) noexcept {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1];
    }

    template <typename T>
    Vector<2u, T> Rotate(const Vector<2u, T>& vec, T angle) noexcept {
        auto cos{ std::cos(angle) };
        auto sin{ std::sin(angle) };

        const T x{ vec[0] * cos + vec[1] * -sin };
        const T y{ vec[0] * sin + vec[1] * cos };

        return Vector<2u, T>{ x, y };
    }
} // namespace adh
