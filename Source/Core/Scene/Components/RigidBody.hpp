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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#pragma once

#include <Std/Array.hpp>
#include <Vertex.hpp>

#include "Math/Math.hpp"
#include "Transform.hpp"

#include <PxPhysicsAPI.h>
#include <btBulletDynamicsCommon.h>

#include <Physics/Physics.hpp>

class btRigidBody;
class btCollisionShape;

namespace adh {
    class RigidBody {
      public:
        enum class BodyType {
            eStatic,
            eKinematic,
            eDynamic
        };

        enum class ColliderType {
            eBox,
            eSphere,
            eCone,
            eCylinder,
            eCapsule,
            eMesh,
            eInvalid,
        };

        RigidBody();

        RigidBody(const RigidBody& rhs) = delete;

        RigidBody& operator=(const RigidBody& rhs) = delete;

        RigidBody(RigidBody&& rhs) noexcept;

        RigidBody& operator=(RigidBody&& rhs) noexcept;

        ~RigidBody();

        void Create(btDiscreteDynamicsWorld* world, ColliderType colliderType, std::uint64_t entity, const Transform& transform,
                    float mass, float bounciness, float friction, const Array<Vertex>& vertices = {});

        void Create(Physics* world, ColliderType colliderType, std::uint64_t entity, const Transform& transform,
                    float mass, float bounciness, float friction, const Array<Vertex>& vertices = {});

        void OnUpdate(Transform& transform) noexcept;

        void SetTranslation(float x, float y, float z) noexcept;

        Vector3D& GetTranslation() noexcept;

        void SetRotation(float x, float y, float z) noexcept;

        void AddRotation(float x, float y, float z) noexcept;

        Vector3D& GetRotation() noexcept;

        void SetVelocity(float x, float y, float z) noexcept;

        void AddVelocity(float x, float y, float z) noexcept;

        Vector3D& GetVelocity() noexcept;

        void SetAngularVelocity(float x, float y, float z) noexcept;

        void AddAngularVelocity(float x, float y, float z) noexcept;

        Vector3D& GetAngularVelocity() noexcept;

        void SetGravity(float x, float y, float z) noexcept;

        bool GetIsTrigger() const noexcept;

        void SetBodyType(BodyType collisionType) noexcept;

        void SetIsTrigger(bool isTrigger) noexcept;

        void SetLinearFactor(float x, float y, float z) noexcept;

        void SetAngularFactor(float x, float y, float z) noexcept;

        void ClearGravity() noexcept;

        void ClearForces() noexcept;

        void Destroy() noexcept;

      private:
        void MoveConstruct(RigidBody&& rhs) noexcept;

        void Clear() noexcept;

      public:
        // btRigidBody* body;
        // btDiscreteDynamicsWorld* m_World;
        float mass;
        float bounciness;
        float friction;

        physx::PxRigidDynamic* mActor;
        physx::PxMaterial* mMaterial;
        physx::PxShape* mShape;

        Physics* mPhysics;

        std::uint64_t entity;

        bool isTrigger{ false };
        bool isKinematic{ false };

        ColliderType colliderType;

        Vector3D velocity{};
        Vector3D angularVelocity{};

        Vector3D translate;
        Vector3D rotation;
        Vector3D scale;
    };
} // namespace adh
