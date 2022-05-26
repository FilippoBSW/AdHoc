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
#include <Math/Math.hpp>

namespace adh {
    struct Camera2D {
        Camera2D() = default;

        Camera2D(const Vector3D& eyePosition, const Vector3D& focusPosition, const Vector3D& upVector,
                 float left, float right, float bottom, float top, float nearZ, float farZ)
            : eyePosition{ eyePosition },
              focusPosition{ focusPosition },
              upVector{ upVector },
              left{ left },
              right{ right },
              bottom{ bottom },
              top{ top },
              nearZ{ nearZ },
              farZ{ farZ } {}

        Matrix4D GetView() const noexcept {
            return LookAtLH(eyePosition, focusPosition, upVector);
        }

        xmm::Matrix GetXmmView() const noexcept {
            return xmm::LookAtLH(eyePosition, focusPosition, upVector);
        }

        Matrix4D GetProjection() {
            return OrthographicLH(left, right, bottom, top, nearZ, farZ);
        }

        xmm::Matrix GetXmmProjection() {
            return xmm::OrthographicLH(left, right, bottom, top, nearZ, farZ);
        }

        Vector3D eyePosition{ 0.0f, 0.0f, 0.0f };
        Vector3D focusPosition{ 0.0f, 0.0f, 0.0f };
        Vector3D upVector{ 0.0f, 1.0f, 0.0f };

        float left{ 0.0f };
        float right{ 1.0f };
        float bottom{ 0.0f };
        float top{ 1.0f };
        float nearZ{ 1.0f };
        float farZ{ 100.0f };

        bool isSceneCamera{ false };
        bool isRuntimeCamera{ false };
    };

    struct Camera3D {
        Camera3D() = default;

        Camera3D(const Vector3D& eyePosition, const Vector3D& focusPosition, const Vector3D& upVector,
                 float fieldOfView, float aspectRatio, float nearZ, float farZ)
            : eyePosition{ eyePosition },
              focusPosition{ focusPosition },
              upVector{ upVector },
              fieldOfView{ fieldOfView },
              aspectRatio{ aspectRatio },
              nearZ{ nearZ },
              farZ{ farZ } {}

        Matrix4D GetView() const noexcept {
            return LookAtLH(eyePosition, focusPosition, upVector);
        }

        xmm::Matrix GetXmmView() const noexcept {
            return xmm::LookAtLH(eyePosition, focusPosition, upVector);
        }

        Matrix4D GetProjection() {
            return PerspectiveLH(ToRadians(fieldOfView), aspectRatio, nearZ, farZ);
        }

        xmm::Matrix GetXmmProjection() {
            return xmm::PerspectiveLH(ToRadians(fieldOfView), aspectRatio, nearZ, farZ);
        }

        Vector3D eyePosition{ 0.0f, 0.0f, 0.0f };
        Vector3D focusPosition{ 0.0f, 0.0f, 0.0f };
        Vector3D upVector{ 0.0f, 1.0f, 0.0f };

        float fieldOfView{ 45.0f };
        float aspectRatio{ 1.0f };
        float nearZ{ 1.0f };
        float farZ{ 100.0f };

        bool isSceneCamera{ false };
        bool isRuntimeCamera{ false };
    };
} // namespace adh
