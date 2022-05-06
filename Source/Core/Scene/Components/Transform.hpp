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
