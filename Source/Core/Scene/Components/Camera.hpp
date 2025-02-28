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
