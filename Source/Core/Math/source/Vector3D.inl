

namespace adh {
    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Add(const Vector<3u, T>& rhs) noexcept {
        return *this = adh::Add(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Subtract(const Vector<3u, T>& rhs) noexcept {
        return *this = adh::Subtract(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Multiply(T rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Multiply(const Matrix<3u, 3u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Multiply(const Vector<3u, T>& rhs) noexcept {
        return *this = adh::Multiply(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Divide(T rhs) noexcept {
        return *this = adh::Divide(*this, rhs);
    }

    template <typename T>
    T Vector<3u, T>::Dot(const Vector<3u, T>& rhs) const noexcept {
        return adh::Dot(*this, rhs);
    }

    template <typename T>
    Vector<3u, T> adh::Vector<3u, T>::Cross(const Vector<3u, T>& rhs) const noexcept {
        return adh::Cross(*this, rhs);
    }

    template <typename T>
    T Vector<3u, T>::Magnitude() const noexcept {
        return adh::Magnitude(*this);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Normalize() noexcept {
        return *this = adh::Divide(*this, adh::Magnitude(*this));
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Scale(T rhs) noexcept {
        return *this = adh::Scale(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Rotate(T angle, const Vector<3u, T>& axis) noexcept {
        return *this = adh::Rotate(*this, angle, axis);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Rotate(const Vector<3u, T>& angle) noexcept {
        return *this = adh::Rotate(*this, angle);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Reflect(const Vector<3u, T>& rhs) noexcept {
        return *this = adh::Reflect(*this, rhs);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Refract(const Vector<3u, T>& rhs, T n) noexcept {
        return *this = adh::Refract(*this, rhs, n);
    }

    template <typename T>
    Vector<3u, T>& Vector<3u, T>::Negate() noexcept {
        return *this = adh::Negate(*this);
    }

    template <typename T>
    std::size_t Vector<3u, T>::GetSize() const noexcept {
        return 3u;
    }

    template <typename T>
    T& Vector<3u, T>::operator[](std::size_t index) ADH_NOEXCEPT {
        ADH_THROW(index < 3u, "Vector out of range!");
        return v[index];
    }

    template <typename T>
    const T& Vector<3u, T>::operator[](std::size_t index) const ADH_NOEXCEPT {
        ADH_THROW(index < 3u, "Vector out of range!");
        return v[index];
    }
} // namespace adh

namespace adh {
    template <typename T>
    Vector<3u, T> Add(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Vector<3u, T>{ lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] };
    }

    template <typename T>
    Vector<3u, T> Subtract(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Vector<3u, T>{ lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] };
    }

    template <typename T>
    Vector<3u, T> Multiply(const Vector<3u, T>& lhs, T rhs) noexcept {
        return Vector<3u, T>{ lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs };
    }

    template <typename T>
    Vector<3u, T> Multiply(T lhs, const Vector<3u, T>& rhs) noexcept {
        return Multiply(rhs, lhs);
    }

    template <typename T>
    Vector<3u, T> Multiply(const Vector<3u, T>& lhs, const Matrix<3u, 3u, T>& rhs) noexcept {
        return Vector<3u, T>{
            lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0] + lhs[2] * rhs[2][0],
            lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] + lhs[2] * rhs[2][1],
            lhs[0] * rhs[0][2] + lhs[1] * rhs[1][2] + lhs[2] * rhs[2][2]
        };
    }

    template <typename T>
    Vector<3u, T> Multiply(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Vector<3u, T>{ lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] };
    }

    template <typename T>
    Vector<3u, T> Divide(const Vector<3u, T>& lhs, T rhs) noexcept {
        auto d{ (T(1) / rhs) };
        return Vector<3u, T>{ lhs[0] * d, lhs[1] * d, lhs[2] * d };
    }

    template <typename T>
    T Dot(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
    }

    template <typename T>
    Vector<3u, T> Cross(const Vector<3u, T>& lhs, const Vector<3u, T>& rhs) noexcept {
        return Vector<3u, T>{
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
        };
    }
} // namespace adh
