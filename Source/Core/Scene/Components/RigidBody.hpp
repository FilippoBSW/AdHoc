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
#include "Mesh.hpp"
#include "Transform.hpp"
#include <Physics/PhysicsWorld.hpp>

namespace adh {
    enum class PhysicsBodyType {
        eStatic,
        eDynamic
    };

    enum class PhysicsColliderShape {
        eBox,
        eSphere,
        eCapsule,
        eMesh,
        eConvexMesh,
        eInvalid
    };

    enum class PhysicsColliderType {
        eCollider,
        eTrigger
    };

    class RigidBody {
      public:
        RigidBody();

        RigidBody(const RigidBody& rhs) = delete;

        RigidBody& operator=(const RigidBody& rhs) = delete;

        RigidBody(RigidBody&& rhs) noexcept;

        RigidBody& operator=(RigidBody&& rhs) noexcept;

        ~RigidBody();

        void Create(std::uint64_t entity,
                    physx::PxScene* scene,
                    float staticFriction,
                    float dynamicFriction,
                    float restitution,
                    PhysicsBodyType bodyType,
                    float mass,
                    bool isKinematic,
                    bool isTrigger,
                    PhysicsColliderShape colliderShape,
                    PhysicsColliderType colliderType,
                    const Vector3D& scale,
                    float radius,
                    float halfHeight,
                    const Mesh* const mesh = nullptr);

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

        void AddForce(float x, float y, float z) noexcept;

        void AddTorque(float x, float y, float z) noexcept;

        void SetHasGravity(bool hasGravity) noexcept;

        bool GetIsTrigger() const noexcept;

        void SetBodyType() noexcept;

        void SetTrigger(bool isTrigger) noexcept;

        void SetKinematic(bool isKinematic) noexcept;

        void SetGeometry(const physx::PxGeometry& geometry);

        void SetLinearFactor(float x, float y, float z) noexcept;

        void SetAngularFactor(float x, float y, float z) noexcept;

        void ClearForces() noexcept;

        void Destroy() noexcept;

      private:
        void MoveConstruct(RigidBody&& rhs) noexcept;

        void Clear() noexcept;

      public:
        // Material
        float staticFriction;
        float dynamicFriction;
        float restitution;
        physx::PxMaterial* material;

        // Actor
        bool isKinematic{ false };
        float mass{ 1.0f };
        PhysicsBodyType bodyType;
        physx::PxRigidActor* actor;

        // Shape
        PhysicsColliderShape colliderShape;
        PhysicsColliderType colliderType;
        physx::PxShape* shape;
        Vector3D scale;
        float radius;
        float halfHeight;

        std::uint64_t entity;
        physx::PxScene* scene;

        bool isTrigger{ false };

        Vector3D velocity;
        Vector3D angularVelocity;
        Vector3D translate;
        Vector3D rotation;
    };
} // namespace adh
