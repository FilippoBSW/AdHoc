#pragma once

namespace adh {
    template <typename T>
    inline constexpr T PI{ 3.14159265358979 };
    inline constexpr double pi{ PI<double> };

    template <typename T>
    constexpr T ToRadians(T degrees) noexcept {
        return degrees / T(180) * static_cast<T>(pi);
    }

    template <typename T>
    constexpr T ToDegrees(T radiants) noexcept {
        return radiants / static_cast<T>(pi) * T(180);
    }
} // namespace adh
