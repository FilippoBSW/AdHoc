// *********************************************************************************
// MIT License
//
// Copyright (c) 2021-2022 Filippo-BSW
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

#if !defined(ADH_XMM_SHUFFLE_MASK)
#    define ADH_XMM_SHUFFLE_MASK(x, y, z, w) (x | (y << 2) | (z << 4) | (w << 6))
#endif
#if !defined(ADH_XMM_CAST_SHUFFLE)
#    define ADH_XMM_CAST_SHUFFLE(vec, x, y, z, w) _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), ADH_XMM_SHUFFLE_MASK(x, y, z, w)))
#endif

namespace adh {
    namespace xmm {
        inline Vector Multiply2x2(const Vector& lhs, const Vector& rhs) noexcept {
            return lhs * Vector{ ADH_XMM_CAST_SHUFFLE(rhs, 0, 3, 0, 3) } +
                   Vector{ ADH_XMM_CAST_SHUFFLE(lhs, 1, 0, 3, 2) } * Vector{ ADH_XMM_CAST_SHUFFLE(rhs, 2, 1, 2, 1) };
        }

        inline Vector MultiplyAdjugate2x2(const Vector& lhs, const Vector& rhs) noexcept {
            return Vector{ ADH_XMM_CAST_SHUFFLE(lhs, 3, 3, 0, 0) } * rhs -
                   Vector{ ADH_XMM_CAST_SHUFFLE(lhs, 1, 1, 2, 2) } * Vector{ ADH_XMM_CAST_SHUFFLE(rhs, 2, 3, 0, 1) };
        }

        inline Vector AdjugateMultiply2x2(const Vector& lhs, const Vector& rhs) noexcept {
            return lhs * Vector{ ADH_XMM_CAST_SHUFFLE(rhs, 3, 0, 3, 0) } -
                   Vector{ ADH_XMM_CAST_SHUFFLE(lhs, 1, 0, 3, 2) } * Vector{ ADH_XMM_CAST_SHUFFLE(rhs, 2, 1, 2, 1) };
        }
    } // namespace xmm
} // namespace adh

namespace adh {
    namespace xmm {
        Matrix::Matrix() noexcept : m{} {
        }

        Matrix::Matrix(float identityValue) noexcept : m{} {
            m[0][0] = identityValue;
            m[1][1] = identityValue;
            m[2][2] = identityValue;
            m[3][3] = identityValue;
        }

        template <typename... Args, typename>
        Matrix::Matrix(Args&&... args) noexcept : f{ std::forward<Args>(args)... } {
        }

        auto& Matrix::Add(const Matrix& rhs) noexcept {
            return *this = adh::xmm::Add(*this, rhs);
        }

        auto& Matrix::Subtract(const Matrix& rhs) noexcept {
            return *this = adh::xmm::Subtract(*this, rhs);
        }

        auto& Matrix::Multiply(const Matrix& rhs) noexcept {
            return *this = adh::xmm::Multiply(*this, rhs);
        }

        auto& Matrix::Multiply(float rhs) noexcept {
            return *this = adh::xmm::Multiply(*this, rhs);
        }

        void Matrix::Identity() noexcept {
            *this = Matrix{ 1.0f };
        }

        auto& Matrix::Transpose() noexcept {
            return *this = adh::xmm::Transpose(*this);
        }

        auto& Matrix::Inverse() noexcept {
            return *this = adh::xmm::Inverse(*this);
        }

        auto& Matrix::Translate(const Vector3D& rhs) noexcept {
            return *this = adh::xmm::Translate(*this, rhs);
        }

        auto& Matrix::Scale(const Vector3D& rhs) noexcept {
            return *this = adh::xmm::Scale(*this, rhs);
        }

        auto& Matrix::Rotate(float angle, const Vector3D& axis) noexcept {
            return *this = adh::xmm::Rotate(*this, angle, axis);
        }

        auto& Matrix::Rotate(const Vector3D& angle) noexcept {
            return *this = adh::xmm::Rotate(*this, angle);
        }

        void Matrix::Decompose(Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept {
            adh::xmm::Decompose(*this, translation, rotation, scale);
        }

        auto& Matrix::LookAtLH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept {
            return *this = adh::xmm::LookAtLH(eyePos, focusPos, upVector);
        }

        auto& Matrix::LookAtRH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept {
            return *this = adh::xmm::LookAtRH(eyePos, focusPos, upVector);
        }

        auto& Matrix::OrthographicLH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
            return *this = adh::xmm::OrthographicLH(left, right, bottom, top, nearZ, farZ);
        }

        auto& Matrix::OrthographicRH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
            return *this = adh::xmm::OrthographicRH(left, right, bottom, top, nearZ, farZ);
        }

        auto& Matrix::PerspectiveLH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT {
            return *this = adh::xmm::PerspectiveLH(fovY, aspectRatio, nearZ, farZ);
        }

        auto& Matrix::PerspectiveRH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT {
            return *this = adh::xmm::PerspectiveRH(fovY, aspectRatio, nearZ, farZ);
        }

        Vector& Matrix::operator[](std::size_t index) noexcept {
            return m[index];
        }

        const Vector& Matrix::operator[](std::size_t index) const noexcept {
            return m[index];
        }
    } // namespace xmm
} // namespace adh

namespace adh {
    namespace xmm {
        Matrix Add(const Matrix& lhs, const Matrix& rhs) noexcept {
            Matrix returnValue;

            for (std::size_t i{}; i != 4u; ++i) {
                returnValue[i] = lhs[i] + rhs[i];
            }

            return returnValue;
        }

        Matrix Subtract(const Matrix& lhs, const Matrix& rhs) noexcept {
            Matrix returnValue;

            for (std::size_t i{}; i != 4u; ++i) {
                returnValue[i] = lhs[i] - rhs[i];
            }

            return returnValue;
        }

        Matrix Multiply(const Matrix& lhs, const Matrix& rhs) noexcept {
            Matrix returnValue;

            for (std::size_t i{}; i != 4u; ++i) {
                Vector vec{ lhs[0] * rhs[i] };

                for (std::size_t j{ 1u }; j != 4u; ++j) {
                    vec += lhs[j] * rhs[i][j];
                }

                returnValue[i] = vec;
            }

            return returnValue;
        }

        Matrix Multiply(const Matrix& lhs, float rhs) noexcept {
            Matrix returnValue;

            for (std::size_t i{}; i != 4u; ++i) {
                returnValue[i] = lhs[i] * rhs;
            }

            return returnValue;
        }

        Matrix Transpose(const Matrix& lhs) noexcept {
            Matrix m;
            m.m[0] = lhs.m[0];
            m.m[1] = lhs.m[1];
            m.m[2] = lhs.m[2];
            m.m[3] = lhs.m[3];

            Matrix temp;
            temp.m[0] = _mm_shuffle_ps(m.m[0], m.m[1], ADH_XMM_SHUFFLE_MASK(0, 1, 0, 1));
            temp.m[2] = _mm_shuffle_ps(m.m[0], m.m[1], ADH_XMM_SHUFFLE_MASK(2, 3, 2, 3));
            temp.m[1] = _mm_shuffle_ps(m.m[2], m.m[3], ADH_XMM_SHUFFLE_MASK(0, 1, 0, 1));
            temp.m[3] = _mm_shuffle_ps(m.m[2], m.m[3], ADH_XMM_SHUFFLE_MASK(2, 3, 2, 3));

            m.m[0] = _mm_shuffle_ps(temp.m[0], temp.m[1], ADH_XMM_SHUFFLE_MASK(0, 2, 0, 2));
            m.m[1] = _mm_shuffle_ps(temp.m[0], temp.m[1], ADH_XMM_SHUFFLE_MASK(1, 3, 1, 3));
            m.m[2] = _mm_shuffle_ps(temp.m[2], temp.m[3], ADH_XMM_SHUFFLE_MASK(0, 2, 0, 2));
            m.m[3] = _mm_shuffle_ps(temp.m[2], temp.m[3], ADH_XMM_SHUFFLE_MASK(1, 3, 1, 3));

            return m;
        }

        Matrix Inverse(const Matrix& mat) noexcept {
            const Vector A{ _mm_movelh_ps(mat[0], mat[1]) };
            const Vector B{ _mm_movehl_ps(mat[1], mat[0]) };
            const Vector C{ _mm_movelh_ps(mat[2], mat[3]) };
            const Vector D{ _mm_movehl_ps(mat[3], mat[2]) };

            const Vector detSub{
                _mm_shuffle_ps(mat[0], mat[2], ADH_XMM_SHUFFLE_MASK(0, 2, 0, 2)) * _mm_shuffle_ps(mat[1], mat[3], ADH_XMM_SHUFFLE_MASK(1, 3, 1, 3)) -
                _mm_shuffle_ps(mat[0], mat[2], ADH_XMM_SHUFFLE_MASK(1, 3, 1, 3)) * _mm_shuffle_ps(mat[1], mat[3], ADH_XMM_SHUFFLE_MASK(0, 2, 0, 2))
            };

            const Vector detA{ ADH_XMM_CAST_SHUFFLE(detSub, 0, 0, 0, 0) };
            const Vector detB{ ADH_XMM_CAST_SHUFFLE(detSub, 1, 1, 1, 1) };
            const Vector detC{ ADH_XMM_CAST_SHUFFLE(detSub, 2, 2, 2, 2) };
            const Vector detD{ ADH_XMM_CAST_SHUFFLE(detSub, 3, 3, 3, 3) };

            const Vector D_C{ AdjugateMultiply2x2(D, C) };
            const Vector A_B{ AdjugateMultiply2x2(A, B) };

            Vector X{ (detD * A) - Multiply2x2(B, D_C) };
            Vector W{ (detA * D) - Multiply2x2(C, A_B) };
            Vector Y{ (detB * C) - MultiplyAdjugate2x2(D, A_B) };
            Vector Z{ (detC * B) - MultiplyAdjugate2x2(A, D_C) };

            Vector tr{ A_B * Vector{ ADH_XMM_CAST_SHUFFLE(D_C, 0, 2, 1, 3) } };
            tr = _mm_hadd_ps(tr, tr);
            tr = _mm_hadd_ps(tr, tr);

            Vector detM{ detA * detD };
            detM = detM + (detB * detC);
            detM = detM - tr;

            const Vector adjSignMask{ 1.0f, -1.0f, -1.0f, 1.0f };
            const Vector rDetM{ adjSignMask / detM };

            X *= rDetM;
            Y *= rDetM;
            Z *= rDetM;
            W *= rDetM;

            Matrix returnValue;
            returnValue[0] = _mm_shuffle_ps(X, Y, ADH_XMM_SHUFFLE_MASK(3, 1, 3, 1));
            returnValue[1] = _mm_shuffle_ps(X, Y, ADH_XMM_SHUFFLE_MASK(2, 0, 2, 0));
            returnValue[2] = _mm_shuffle_ps(Z, W, ADH_XMM_SHUFFLE_MASK(3, 1, 3, 1));
            returnValue[3] = _mm_shuffle_ps(Z, W, ADH_XMM_SHUFFLE_MASK(2, 0, 2, 0));

            return returnValue;
        }

        Matrix Translate(const Matrix& lhs, const Vector3D& rhs) noexcept {
            Matrix returnValue;
            returnValue[0] = lhs[0];
            returnValue[1] = lhs[1];
            returnValue[2] = lhs[2];
            returnValue[3] = lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3];

            return returnValue;
        }

        Matrix Scale(const Matrix& lhs, const Vector3D& rhs) noexcept {
            Matrix returnValue;
            returnValue[0] = lhs[0] * rhs[0];
            returnValue[1] = lhs[1] * rhs[1];
            returnValue[2] = lhs[2] * rhs[2];
            returnValue[3] = lhs[3];

            return returnValue;
        }

        Matrix Rotate(const Matrix& mat, float angle, const Vector3D& axis) noexcept {
            Quaternion q{ angle, axis };
            return mat * q.GetXmmMatrix();
        }

        Matrix Rotate(const Matrix& mat, const Vector3D& angle) noexcept {
            Quaternion q{ angle };
            return mat * q.GetXmmMatrix();
        }

        inline void Decompose(const Matrix& m, Vector3D& translation, Vector3D& rotation, Vector3D& scale) noexcept {
            // Translation
            translation = Vector3D{ m[3][0], m[3][1], m[3][2] };

            Vector3D rows[3];
            for (std::int32_t i{}; i != 3; ++i) {
                for (std::int32_t j{}; j != 3; ++j) {
                    rows[i][j] = m[i][j];
                }
            }

            // Scale
            scale.x = rows[0].Magnitude();
            rows[0].Scale(1.0f);
            scale.y = rows[1].Magnitude();
            rows[1].Scale(1.0f);
            scale.z = rows[2].Magnitude();
            rows[2].Scale(1.0f);

            // Rotation
            // rotation.y = std::asin(-rows[0][2]);
            // if (std::cos(rotation.y) != 0) {
            //     rotation.x = -std::atan2(rows[1][2], rows[2][2]);
            //     rotation.z = -std::atan2(rows[0][1], rows[0][0]);
            // } else {
            //     rotation.x = -std::atan2(-rows[2][0], rows[1][1]);
            //     rotation.z = 0;
            // }

            rotation.x = asinf(-rows[2][1]); // Pitch
            if (cosf(rotation.x) > 0.0001) {
                rotation.y = atan2f(rows[2][0], rows[2][2]); // Yaw
                rotation.z = atan2f(rows[0][1], rows[1][1]); // Roll
            } else {
                rotation.y = 0.0f;                            // Yaw
                rotation.z = atan2f(-rows[1][0], rows[0][0]); // Roll
            }
        }

        Matrix LookAtLH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept {
            const Vector3D forward{ Normalize(focusPos - eyePos) };
            const Vector3D right{ Normalize(Cross(forward, upVector)) };
            const Vector3D up{ Cross(right, forward) };

            Matrix returnValue{
                -right[0],
                up[0],
                forward[0],
                0.0f,
                -right[1],
                up[1],
                forward[1],
                0.0f,
                -right[2],
                up[2],
                forward[2],
                0.0f,
                Dot(right, eyePos),
                -Dot(up, eyePos),
                -Dot(forward, eyePos),
                1.0f,
            };

            return returnValue;
        }

        Matrix LookAtRH(const Vector3D& eyePos, const Vector3D& focusPos, const Vector3D& upVector) noexcept {
            const Vector3D forward{ Normalize(focusPos - eyePos) };
            const Vector3D right{ Normalize(Cross(forward, upVector)) };
            const Vector3D up{ Cross(right, forward) };

            Matrix returnValue{
                right[0],
                up[0],
                -forward[0],
                0.0f,
                right[1],
                up[1],
                -forward[1],
                0.0f,
                right[2],
                up[2],
                -forward[2],
                0.0f,
                -Dot(right, eyePos),
                -Dot(up, eyePos),
                Dot(forward, eyePos),
                1.0f,
            };

            return returnValue;
        }

        Matrix OrthographicLH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
            Matrix returnValue{};
            returnValue[0][0] = 2.0f / (right - left);
            returnValue[1][1] = 2.0f / (top - bottom);

            if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
                returnValue[2][2] = 1.0f / (farZ - nearZ);
                returnValue[3][0] = -(right + left) / (right - left);
                returnValue[3][1] = -(top + bottom) / (top - bottom);
                returnValue[3][2] = -nearZ / (farZ - nearZ);
            } else {
                returnValue[2][2] = 1.0f / (farZ - nearZ);
                returnValue[3][0] = -(right + left) / (right - left);
                returnValue[3][1] = -(top + bottom) / (top - bottom);
                returnValue[3][2] = -(farZ + nearZ) / (farZ - nearZ);
            }

            returnValue[3][3] = 1.0f;

            return returnValue;
        }

        Matrix OrthographicRH(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept {
            Matrix returnValue{};
            returnValue[0][0] = 2.0f / (right - left);
            returnValue[1][1] = 2.0f / (top - bottom);

            if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
                returnValue[2][2] = -1.0f / (farZ - nearZ);
                returnValue[3][0] = -(right + left) / (right - left);
                returnValue[3][1] = -(top + bottom) / (top - bottom);
                returnValue[3][2] = -nearZ / (farZ - nearZ);
            } else {
                returnValue[2][2] = -2.0f / (farZ - nearZ);
                returnValue[3][0] = -(right + left) / (right - left);
                returnValue[3][1] = -(top + bottom) / (top - bottom);
                returnValue[3][2] = -(farZ + nearZ) / (farZ - nearZ);
            }

            returnValue[3][3] = 1.0f;
            return returnValue;
        }

        Matrix PerspectiveLH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT {
            ADH_THROW(nearZ > 0.0f, "NearZ needs to be bigger than zero!");
            const float tanFov{ std::sin(fovY / 2.0f) / std::cos(fovY / 2.0f) };

            Matrix returnValue{};
            returnValue[0][0] = 1.0f / (aspectRatio * tanFov);
            returnValue[1][1] = 1.0f / tanFov;
            returnValue[2][2] = -(farZ / (nearZ - farZ));
            returnValue[2][3] = 1.0f;

            if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
                returnValue[3][2] = -(farZ * nearZ) / (farZ - nearZ);
            } else {
                returnValue[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
            }

            return returnValue;
        }

        Matrix PerspectiveRH(float fovY, float aspectRatio, float nearZ, float farZ) ADH_NOEXCEPT {
            ADH_THROW(nearZ > 0.0f, "NearZ needs to be bigger than zero!");
            const float tanFov{ std::sin(fovY / 2.0f) / std::cos(fovY / 2.0f) };

            Matrix returnValue{};
            returnValue[0][0] = 1.0f / (aspectRatio * tanFov);
            returnValue[1][1] = 1.0f / tanFov;
            returnValue[2][2] = farZ / (nearZ - farZ);
            returnValue[2][3] = -1.0f;

            if (!ADH_MATH_DEPTH_TO_MINUS_ONE) {
                returnValue[3][2] = -(farZ * nearZ) / (farZ - nearZ);
            } else {
                returnValue[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
            }

            return returnValue;
        }

        Matrix operator+(const Matrix& lhs, const Matrix& rhs) noexcept {
            return Add(lhs, rhs);
        }

        Matrix& operator+=(Matrix& lhs, const Matrix& rhs) noexcept {
            return lhs = lhs + rhs;
        }

        Matrix operator-(const Matrix& lhs, const Matrix& rhs) noexcept {
            return Subtract(lhs, rhs);
        }

        Matrix& operator-=(Matrix& lhs, const Matrix& rhs) noexcept {
            return lhs = lhs - rhs;
        }

        Matrix operator*(const Matrix& lhs, const Matrix& rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Matrix& operator*=(Matrix& lhs, const Matrix& rhs) noexcept {
            return lhs = lhs * rhs;
        }

        Matrix operator*(const Matrix& lhs, float rhs) noexcept {
            return Multiply(lhs, rhs);
        }

        Matrix& operator*=(Matrix& lhs, float rhs) noexcept {
            return lhs = lhs * rhs;
        }
    } // namespace xmm
} // namespace adh
