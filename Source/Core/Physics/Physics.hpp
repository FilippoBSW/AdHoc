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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#pragma once
#include <Math/Math.hpp>
#include <btBulletDynamicsCommon.h>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace adh {
    class Physics {
      public:
        Physics();

        Physics(const Physics& rhs) = delete;

        Physics& operator=(const Physics& rhs) = delete;

        Physics(Physics&& rhs) noexcept;

        Physics& operator=(Physics&& rhs) noexcept;

        ~Physics();

        void Create();

        void Destroy();

        void OnUpdate(float deltaTime);

        void SetGravity(const Vector3D& gravity) noexcept;

        void ClearForces() noexcept;

      private:
        void StepSimulation(float deltaTime);

        void MoveConstruct(Physics&& rhs) noexcept;

        void Clear() noexcept;

      public:
        btBroadphaseInterface* m_Broadphase;
        btDefaultCollisionConfiguration* m_CollisionConfiguration;
        btCollisionDispatcher* m_Dispatcher;
        btSequentialImpulseConstraintSolver* m_Solver;
        btDiscreteDynamicsWorld* m_World;

        btVector3 m_Gravity{ 0.0f, -9.8f, 0.0f };
    };
} // namespace adh
