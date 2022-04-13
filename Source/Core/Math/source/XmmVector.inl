// *********************************************************************************
// MIT License
//
// Copyright (c) 2021 Filippo-BSW
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#pragma once

namespace adh {
    namespace xmm {
        Vector::Vector() noexcept : v{} {
        }

        Vector::Vector(__m128 vec) noexcept : v{ vec } {
        }

        Vector::Vector(float rhs) noexcept {
            *this = adh::xmm::Set(rhs);
        }

        Vector::Vector(float x, float y, float z, float w) noexcept {
            *this = adh::xmm::Set(x, y, z, w);
        }

        Vector::Vector(const float* rhs) noexcept {
            *this = adh::xmm::Load(rhs);
        }

        Vector& Vector::Set(float rhs) noexcept {
            return *this = adh::xmm::Set(rhs);
        }

        Vector& Vector::Set(float x, float y, float z, float w) noexcept {
            return *this = adh::xmm::Set(x, y, z, w);
        }

        Vector& Vector::Load(const float* rhs) noexcept {
            return *this = adh::xmm::Load(rhs);
        }

        Vector& Vector::Store(float* rhs) noexcept {
            return *this = adh::xmm::Store(rhs);
        }

        Vector& Vector::Add(const Vector& rhs) noexcept {
            return *this = adh::xmm::Add(*this, rhs);
        }

        Vector& Vector::Subtract(const Vector& rhs) noexcept {
            return *this = adh::xmm::Subtract(*this, rhs);
        }

        Vector& Vector::Multiply(const Vector& rhs) noexcept {
            return *this = adh::xmm::Multiply(*this, rhs);
        }

        Vector& Vector::Multiply(float rhs) noexcept {
            return *this = adh::xmm::Multiply(*this, rhs);
        }

        Vector& Vector::Multiply(const Matrix& rhs) noexcept {
            return *this = adh::xmm::Multiply(*this, rhs);
        }

        Vector& Vector::Divide(const Vector& rhs) noexcept {
            return *this = adh::xmm::Divide(*this, rhs);
        }

        Vector& Vector::Divide(float rhs) noexcept {
            return *this = adh::xmm::Divide(*this, rhs);
        }

        float Vector::Dot(const Vector& rhs) const noexcept {
            return adh::xmm::Dot(*this, rhs);
        }

        Vector Vector::Cross(const Vector& rhs) const noexcept {
            return adh::xmm::Cross(*this, rhs);
        }

        float Vector::Magnitude() const noexcept {
            return adh::xmm::Magnitude(*this);
        }

        Vector& Vector::Normalize() noexcept {
            return *this = adh::xmm::Normalize(*this);
        }

        Vector& Vector::Negate() noexcept {
            return *this = adh::xmm::Negate(*this);
        }

        Vector& Vector::Rotate(float angle, const Vector3D& axis) noexcept {
            return *this = adh::xmm::Rotate(*this, angle, axis);
        }

        Vector& Vector::Rotate(const Vector3D& angle) noexcept {
            return *this = adh::xmm::Rotate(*this, angle);
        }

        Vector& Vector::Reflect(const Vector& rhs) noexcept {
            return *this = adh::xmm::Reflect(*this, rhs);
        }

        Vector& Vector::Refract(const Vector& rhs, float n) noexcept {
            return *this = adh::xmm::Refract(*this, rhs, n);
        }

        float& Vector::operator[](std::size_t index) ADH_NOEXCEPT {
            ADH_THROW(index < 4u, "Vector out of range!");
            return f[index];
        }

        const float& Vector::operator[](std::size_t index) const ADH_NOEXCEPT {
            ADH_THROW(index < 4u, "Vector out of range!");
            return f[index];
        }

        Vector::operator __m128() {
            return v;
        }

        Vector::operator const __m128() const {
            return v;
        }
    } // namespace xmm
} // namespace adh

namespace adh {
    namespace xmm {
        Vector Set(float rhs) noexcept {
            return _mm_set1_ps(rhs);
        }

        Vector Set(float x, float y, float z, float w) noexcept {
            return _mm_set_ps(w, z, y, x);
        }

        Vector Load(const float* rhs) noexcept {
            return _mm_load_ps(rhs);
        }

        Vector Store(float* rhs) noexcept {
            Vector returnValue{};
            _mm_store_ps(rhs, returnValue);

            return returnValue;
        }

        Vector Add(const Vector& lhs, const Vector& rhs) noexcept {
            return _mm_add_ps(lhs, rhs);
        }

        Vector Subtract(const Vector& lhs, const Vector& rhs) noexcept {
            return _mm_sub_ps(lhs, rhs);
        }

        Vector Multiply(const Vector& lhs, const Vector& rhs) noexcept {
            return _mm_mul_ps(lhs, rhs);
        }

        Vector Multiply(const Vector& lhs, float rhs) noexcept {
            return Multiply(lhs, Vector{ rhs });
        }

        Vector Multiply(const Vector& lhs, const Matrix& rhs) noexcept {
            Vector returnValue{ Vector{ rhs.m[0] * lhs[0] } };

            for (std::size_t i{ 1u }; i != 4u; ++i) {
                returnValue += Vector{ rhs.m[i] } * Vector{ lhs[i] };
            }

            return returnValue;
        }

        Vector Divide(const Vector& lhs, const Vector& rhs) noexcept {
            return _mm_div_ps(lhs, rhs);
        }

        Vector Divide(const Vector& lhs, float rhs) noexcept {
            return Divide(lhs, Vector{ rhs });
        }

        float Dot(const Vector& lhs, const Vector& rhs) noexcept {
            return Vector{ _mm_dp_ps(lhs, rhs, 0xff) }[0];
        }

        Vector Cross(const Vector& lhs, const Vector& rhs) noexcept {
            return Vector{ _mm_sub_ps(
                _mm_mul_ps(_mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 1, 0, 2))),
                _mm_mul_ps(_mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1)))) };
        }

        float Magnitude(const Vector& vec) noexcept {
            return std::sqrt(Dot(vec, vec));
        }

        Vector Normalize(const Vector& vec) noexcept {
            return Divide(vec, Magnitude(vec));
        }

        Vector Negate(const Vector& vec) noexcept {
            return Multiply(vec, -1.0f);
        }

        Vector Reflect(const Vector& lhs, const Vector& rhs) noexcept {
            const Vector v1{ lhs };
            const Vector v2{ Normalize(rhs) };
            const float dot{ Dot(v2, v1) };

            return Vector{ v1 - v2 * dot * 2.0f };

            return Vector{};
        }

        Vector Refract(const Vector& lhs, const Vector& rhs, float n) noexcept {
            const Vector v1{ lhs };
            const Vector v2{ Normalize(rhs) };
            const float dot{ Dot(v2, v1) };
            const float k{ 1.0f - n * n * (1.0f - dot * dot) };

            return Vector{ n * v1 - (n * dot + std::sqrt(k)) * v2 };
        }

        Vector Rotate(const Vector& vec, float angle, const Vector3D& axis) noexcept {
            Quaternion q{ angle, axis };
            return q.GetVector(vec);
        }

        Vector Rotate(const Vector& vec, const Vector3D& angle) noexcept {
            Quaternion q{ angle };
            return q.GetVector(vec);
        }

        Vector operator+(const Vector& lhs, const Vector& rhs) noexcept {
            return Add(lhs, rhs);
        }

        Vector& operator+=(Vector& lhs, const Vector& rhs) noexcept {
            return lhs = lhs + rhs;
        }

        Vector operator-(const Vector& lhs, const Vector& rhs) noexcept {
            return Subtract(lhs, rhs);
        }

        Vector operator-(const Vector& vec) noexcept {
            return Negate(vec);
        }

        Vector& operator-=(Vector& lhs, const Vector& rhs) noexcept {
            return lhs = lhs - rhs;
        }

        Vector operator*(const Vector& lhs, const Vector& rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Vector& operator*=(Vector& lhs, const Vector& rhs) noexcept {
            return lhs = lhs * rhs;
        }

        Vector operator*(const Vector& lhs, float rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Vector operator*(float lhs, const Vector& rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Vector operator*=(Vector& lhs, float rhs) noexcept {
            return lhs = lhs * rhs;
        }

        Vector operator*(const Vector& lhs, const Matrix& rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Vector operator*=(Vector& lhs, const Matrix& rhs) noexcept {
            return lhs = lhs * rhs;
        }

        Vector operator/(const Vector& lhs, const Vector& rhs) noexcept {
            return Divide(lhs, rhs);
        }

        Vector& operator/=(Vector& lhs, const Vector& rhs) noexcept {
            return lhs = lhs / rhs;
        }
    } // namespace xmm
} // namespace adh
