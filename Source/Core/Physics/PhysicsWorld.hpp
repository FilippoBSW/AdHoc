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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CL, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#pragma once
#include <PxPhysicsAPI.h>

#include <Math/Math.hpp>
#include <Std/Array.hpp>

#include <Scene/Components/Mesh.hpp>

namespace adh {
    class PhysicsWorld : public physx::PxSimulationEventCallback {
      private:
        inline static PhysicsWorld* s_This;

      public:
        static PhysicsWorld* Get() ADH_NOEXCEPT {
            ADH_THROW(s_This != nullptr, "Physics world pointer is null!");
            return s_This;
        }

      public:
        PhysicsWorld() = default;

        PhysicsWorld(const PhysicsWorld& rhs) = delete;

        PhysicsWorld& operator=(const PhysicsWorld& rhs) = delete;

        PhysicsWorld(PhysicsWorld&& rhs);

        PhysicsWorld& operator=(PhysicsWorld&& rhs);

        ~PhysicsWorld();

        void Create();

        physx::PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution);

        physx::PxShape* CreateBoxShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const physx::PxVec3& scale);

        physx::PxShape* CreateSphereShape(physx::PxRigidActor* actor, physx::PxMaterial* material, float radius);

        physx::PxShape* CreateCapsuleShape(physx::PxRigidActor* actor, physx::PxMaterial* material, float radius, float halfHeight);

        physx::PxShape* CreateMeshShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const Mesh& mesh);

        physx::PxShape* CreateConvexMeshShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const Mesh& mesh);

        physx::PxRigidDynamic* CreateDynamicActor();

        physx::PxRigidStatic* CreateStaticActor();

        physx::PxScene* GetScene() noexcept;

        void ResetScene();

        void Destroy() noexcept;

        void StepSimulation(float deltaTime);

        Vector3D& GetGravity() noexcept;

        void SetGravity(const Vector3D& gravity) noexcept;

        std::uint64_t Raycast(const Vector3D& from, const Vector3D& direction, float distance);

        // TODO:
        // void Sweep();

        // TODO:
        // void Overlap();

        void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;

        void onWake(physx::PxActor** actors, physx::PxU32 count) override;

        void onSleep(physx::PxActor** actors, physx::PxU32 count) override;

        void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;

        void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

        void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;

      private:
        void MoveConstruct(PhysicsWorld&& rhs) noexcept;

        void Clear() noexcept;

      private:
        physx::PxDefaultAllocator m_DefaultAllocatorCallback;
        physx::PxDefaultErrorCallback m_DefaultErrorCallback;
        physx::PxDefaultCpuDispatcher* m_Dispatcher;
        physx::PxFoundation* m_Foundation;
        physx::PxCooking* m_Cooking;
        physx::PxPhysics* m_Physics;
        Array<physx::PxScene*> m_Scene;
        std::int32_t m_CurrentScene{ 0 };
        Vector3D m_Gravity{ 0.0f, -9.81f, 0.0f };
    };
} // namespace adh