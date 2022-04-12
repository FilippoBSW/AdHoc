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

#include "PhysicsWorld.hpp"
#include <Utility.hpp>

#include <Event/Event.hpp>

#include <thread>

using namespace physx;

namespace adh {
    PhysicsWorld::PhysicsWorld(PhysicsWorld&& rhs) {
        MoveConstruct(Move(rhs));
    }

    PhysicsWorld& PhysicsWorld::operator=(PhysicsWorld&& rhs) {
        Clear();
        MoveConstruct(Move(rhs));
        return *this;
    }

    PhysicsWorld::~PhysicsWorld() {
        Clear();
    }

    void PhysicsWorld::Create() {
        m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallback, m_DefaultErrorCallback);
        ADH_THROW(m_Foundation != 0, "PxCreateFoundation failed!");
        m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale());
        ADH_THROW(m_Physics != 0, "PxCreatePhysics failed!");
        m_Dispatcher = PxDefaultCpuDispatcherCreate(std::thread::hardware_concurrency());
        ADH_THROW(m_Dispatcher != 0, "PxDefaultCpuDispatcherCreate failed!");
        m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, physx::PxCookingParams(PxTolerancesScale()));
        ADH_THROW(m_Cooking != 0, "PxCreateCooking failed!");

        PxTolerancesScale scale;
        PxCookingParams params(scale);
        m_Cooking->setParams(params);

        physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity       = PxVec3(m_Gravity.x, m_Gravity.y, m_Gravity.z);
        sceneDesc.cpuDispatcher = m_Dispatcher;
        sceneDesc.filterShader  = PxDefaultSimulationFilterShader;
        m_Scene.EmplaceBack(m_Physics->createScene(sceneDesc));
        SetGravity(m_Gravity);

        s_This = this;
    }

    physx::PxMaterial* PhysicsWorld::CreateMaterial(float staticFriction, float dynamicFriction, float restitution) {
        return m_Physics->createMaterial(staticFriction, dynamicFriction, restitution);
    }

    physx::PxShape* PhysicsWorld::CreateBoxShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const physx::PxVec3& scale) {
        return physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(scale.x, scale.y, scale.z), &material, 1);
    }

    physx::PxShape* PhysicsWorld::CreateSphereShape(physx::PxRigidActor* actor, physx::PxMaterial* material, float radius) {
        return physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(radius), &material, 1);
    }

    physx::PxShape* PhysicsWorld::CreateCapsuleShape(physx::PxRigidActor* actor, physx::PxMaterial* material, float radius, float halfHeight) {
        return physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxCapsuleGeometry(radius, halfHeight), &material, 1);
    }

    physx::PxShape* PhysicsWorld::CreateMeshShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const Mesh& mesh) {
        PxTriangleMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.data      = mesh.vertex.GetData();
        meshDesc.points.stride    = sizeof(mesh.vertex[0]);
        meshDesc.points.count     = static_cast<physx::PxU32>(mesh.vertex.GetSize());
        meshDesc.triangles.count  = static_cast<physx::PxU32>(mesh.index.GetSize() / 3u);
        meshDesc.triangles.stride = 3 * sizeof(mesh.index[0]);
        meshDesc.triangles.data   = mesh.index.GetData();
        if (sizeof(mesh.index[0]) == sizeof(uint16_t)) {
            meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
        }

        physx::PxTriangleMesh* triMesh = m_Cooking->createTriangleMesh(meshDesc, m_Physics->getPhysicsInsertionCallback());
        auto shape{ PxRigidActorExt::createExclusiveShape(*actor, PxTriangleMeshGeometry(triMesh), *material) };
        triMesh->release();
        return shape;
    }

    physx::PxShape* PhysicsWorld::CreateConvexMeshShape(physx::PxRigidActor* actor, physx::PxMaterial* material, const Mesh& mesh) {
        PxBoundedData pointdata{};
        pointdata.count  = static_cast<physx::PxU32>(mesh.vertex.GetSize());
        pointdata.stride = sizeof(PxVec3);
        pointdata.data   = mesh.vertex.GetData();

        PxConvexMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.count = static_cast<physx::PxU32>(mesh.vertex.GetSize());
        meshDesc.points       = pointdata;
        meshDesc.flags        = PxConvexFlag::eCOMPUTE_CONVEX;

        physx::PxConvexMesh* convMesh = m_Cooking->createConvexMesh(meshDesc, m_Physics->getPhysicsInsertionCallback());
        auto shape                    = PxRigidActorExt::createExclusiveShape(*actor, PxConvexMeshGeometry(convMesh), *material);
        convMesh->release();
        return shape;
    }

    physx::PxRigidDynamic* PhysicsWorld::CreateDynamicActor() {
        physx::PxRigidDynamic* actor = m_Physics->createRigidDynamic(physx::PxTransform{ physx::PxVec3{} });
        m_Scene[m_CurrentScene]->addActor(*actor);
        return actor;
    }

    physx::PxRigidStatic* PhysicsWorld::CreateStaticActor() {
        physx::PxRigidStatic* actor = m_Physics->createRigidStatic(physx::PxTransform{ physx::PxVec3{} });
        m_Scene[m_CurrentScene]->addActor(*actor);
        return actor;
    }

    physx::PxScene* PhysicsWorld::GetScene() noexcept {
        return m_Scene[m_CurrentScene];
    }

    void PhysicsWorld::ResetScene() {
        m_Scene[m_CurrentScene]->release();
        physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
        sceneDesc.gravity       = PxVec3(m_Gravity.x, m_Gravity.y, m_Gravity.z);
        sceneDesc.cpuDispatcher = m_Dispatcher;
        sceneDesc.filterShader  = PxDefaultSimulationFilterShader;
        m_Scene[m_CurrentScene] = m_Physics->createScene(sceneDesc);
        SetGravity(m_Gravity);
    }

    void PhysicsWorld::Destroy() noexcept {
        Clear();
    }

    // TODO: tick, multithread
    void PhysicsWorld::StepSimulation(float deltaTime) {
        m_Scene[m_CurrentScene]->simulate(deltaTime);
        m_Scene[m_CurrentScene]->fetchResults(true);
    }

    Vector3D PhysicsWorld::GetGravity() noexcept {
        return m_Gravity;
    }

    void PhysicsWorld::SetGravity(const Vector3D& gravity) noexcept {
        m_Gravity = gravity;
        m_Scene[m_CurrentScene]->setGravity(physx::PxVec3{ gravity.x, gravity.y, gravity.z });
    }

    void* PhysicsWorld::Raycast(const Vector3D& from, const Vector3D& direction, float distance) {
        PxRaycastBuffer hit;
        if (m_Scene[m_CurrentScene]->raycast(PxVec3{ from.x, from.y, from.z }, PxVec3{ direction.x, direction.y, direction.z }, distance, hit)) {
            return hit.block.actor->userData;
        } else {
            return nullptr;
        }
    }

    void PhysicsWorld::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) {
    }

    void PhysicsWorld::onWake(physx::PxActor** actors, physx::PxU32 count) {
    }

    void PhysicsWorld::onSleep(physx::PxActor** actors, physx::PxU32 count) {
    }

    void PhysicsWorld::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
        std::cout << " Contaxt!" << std::endl;
    }

    void PhysicsWorld::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) {
    }

    void PhysicsWorld::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) {
    }

    void PhysicsWorld::MoveConstruct(PhysicsWorld&& rhs) noexcept {
        m_Cooking      = rhs.m_Cooking;
        m_Dispatcher   = rhs.m_Dispatcher;
        m_Foundation   = rhs.m_Foundation;
        m_Physics      = rhs.m_Physics;
        m_CurrentScene = rhs.m_CurrentScene;
        m_Gravity      = rhs.m_Gravity;
        m_Scene        = Move(rhs.m_Scene);

        rhs.m_Cooking    = nullptr;
        rhs.m_Dispatcher = nullptr;
        rhs.m_Physics    = nullptr;
        rhs.m_Foundation = nullptr;
    }

    void PhysicsWorld::Clear() noexcept {
        for (std::size_t i{}; i != m_Scene.GetSize(); ++i) {
            m_Scene[i]->release();
        }
        m_Scene = {};

        if (m_Cooking) {
            m_Cooking->release();
        }
        if (m_Physics) {
            m_Physics->release();
        }
        if (m_Dispatcher) {
            m_Dispatcher->release();
        }
        if (m_Foundation) {
            m_Foundation->release();
        }
    }
} // namespace adh