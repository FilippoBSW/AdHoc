#pragma once
#include <Math/Math.hpp>

namespace adh {
    struct Transform {
        Transform() = default;
        Transform(Vector3D translate, Vector3D rotation, Vector3D scale)
            : translate{ translate },
              rotation{ rotation },
              scale{ scale } {}

        Vector3D translate{};
        Vector3D rotation{};
        Vector3D scale{ 1.0f, 1.0f, 1.0f };

        Vector3D forward{};

        Quaternion<float> q;

        Vector3D& GetForward() noexcept {
            float pitch{ rotation[0] };
            float yaw{ rotation[1] + ToRadians(90.0f) };

            forward[0] = -(cos(yaw) * cos(pitch));
            forward[1] = -(sin(pitch));
            forward[2] = sin(yaw) * cos(pitch);

            return forward;
        }

        Matrix4D GetPhysics() noexcept {
            Matrix4D ret{ 1.0f };
            ret.Translate(translate);

            // TODO: Temp
            auto m = q.GetMatrix4D();
            Vector3D t, r, s;
            m.Decompose(t, r, s);
            rotation = r;
            ret      = ret * m;

            ret.Scale(scale);
            return ret;
        }

        xmm::Matrix GetXmmPhysics() noexcept {
            xmm::Matrix ret{ 1.0f };
            ret.Translate(translate);

            // TODO: Temp
            auto m = q.GetXmmMatrix();
            Vector3D t, r, s;
            m.Decompose(t, r, s);
            rotation = r;
            ret      = ret * m;

            ret.Scale(scale);
            return ret;
        }

        Matrix4D Get() noexcept {
            Matrix4D ret{ 1.0f };
            ret.Translate(translate);
            ret.Rotate(rotation);
            ret.Scale(scale);
            return ret;
        }

        xmm::Matrix GetXmm() noexcept {
            xmm::Matrix ret{ 1.0f };
            ret.Translate(translate);
            ret.Rotate(rotation);
            ret.Scale(scale);
            return ret;
        }
    };
} // namespace adh
