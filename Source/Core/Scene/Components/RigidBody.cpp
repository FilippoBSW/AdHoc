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

#include "RigidBody.hpp"
#include <Event/Event.hpp>
#include <Math/Math.hpp>
#include <iostream>

namespace adh {
    RigidBody::RigidBody() : actor{},
                             material{},
                             shape{} {}

    RigidBody::RigidBody(RigidBody&& rhs) noexcept {
        MoveConstruct(Move(rhs));
    }

    RigidBody& RigidBody::operator=(RigidBody&& rhs) noexcept {
        Clear();
        MoveConstruct(Move(rhs));
        return *this;
    }

    RigidBody::~RigidBody() {
        Clear();
    }

    void RigidBody::Create(std::uint64_t entity,
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
                           const Mesh* const mesh) {
        this->entity = entity;
        this->scene  = scene;

        // Material
        this->staticFriction  = staticFriction;
        this->dynamicFriction = dynamicFriction;
        this->restitution     = restitution;
        material              = PhysicsWorld::Get()->CreateMaterial(staticFriction, dynamicFriction, restitution);

        // Actor
        this->bodyType    = bodyType;
        this->mass        = mass;
        this->isKinematic = isKinematic;
        switch (bodyType) {
        case PhysicsBodyType::eDynamic:
            {
                actor = PhysicsWorld::Get()->CreateDynamicActor();
                physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(actor), mass);
                static_cast<physx::PxRigidDynamic*>(actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, isKinematic);
                break;
            }
        case PhysicsBodyType::eStatic:
            {
                actor = PhysicsWorld::Get()->CreateStaticActor();
                break;
            }
        }

        // Shape
        this->colliderShape = colliderShape;
        this->colliderType  = colliderType;
        this->isTrigger     = isTrigger;
        this->scale         = scale;
        this->radius        = radius;
        this->halfHeight    = halfHeight;

        switch (colliderShape) {
        case PhysicsColliderShape::eBox:
            {
                shape = PhysicsWorld::Get()->CreateBoxShape(actor, material, physx::PxVec3(scale.x, scale.y, scale.z));
                break;
            }
        case PhysicsColliderShape::eSphere:
            {
                shape = PhysicsWorld::Get()->CreateSphereShape(actor, material, radius);
                break;
            }
        case PhysicsColliderShape::eCapsule:
            {
                shape = PhysicsWorld::Get()->CreateCapsuleShape(actor, material, radius, halfHeight);
                break;
            }
        case PhysicsColliderShape::eMesh:
            {
                shape = PhysicsWorld::Get()->CreateMeshShape(actor, material, *mesh);
                break;
            }
        case PhysicsColliderShape::eConvexMesh:
            {
                shape = PhysicsWorld::Get()->CreateConvexMeshShape(actor, material, *mesh);
                break;
            }
        }
        SetTrigger(isTrigger);

        actor->userData = this;
        actor->attachShape(*shape);
        // scene->addActor(*actor);
    }

    void RigidBody::OnUpdate(Transform& transform) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        transform.translate  = Vector3D{ t.p.x, t.p.y, t.p.z };
        transform.q.x        = t.q.x;
        transform.q.y        = t.q.y;
        transform.q.z        = t.q.z;
        transform.q.w        = t.q.w;

        translate = transform.translate;
        rotation  = transform.rotation;

        if (bodyType == PhysicsBodyType::eDynamic) {
            auto v          = static_cast<physx::PxRigidDynamic*>(actor)->getLinearVelocity();
            velocity        = Vector3D{ v.x, v.y, v.z };
            auto av         = static_cast<physx::PxRigidDynamic*>(actor)->getAngularVelocity();
            angularVelocity = Vector3D{ av.x, av.y, av.z };
        }
    }

    void RigidBody::SetTranslation(float x, float y, float z) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        t.p                  = physx::PxVec3{ x, y, z };
        actor->setGlobalPose(t);
    }

    Vector3D& RigidBody::GetTranslation() noexcept {
        return translate;
    }

    void RigidBody::SetRotation(float x, float y, float z) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        Quaternion qq(Vector3D{ x, y, z });
        physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
        t.q = q;
        actor->setGlobalPose(t);
    }

    void RigidBody::AddRotation(float x, float y, float z) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        Quaternion qq(Vector3D{ x, y, z });
        physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
        t.q *= q;
        actor->setGlobalPose(t);
    }

    Vector3D& RigidBody::GetRotation() noexcept {
        return rotation;
    }

    void RigidBody::SetVelocity(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(physx::PxVec3(x, y, z));
    }

    void RigidBody::AddVelocity(float x, float y, float z) noexcept {
        auto vel{ static_cast<physx::PxRigidDynamic*>(actor)->getLinearVelocity() };
        vel.x += x;
        vel.y += y;
        vel.z += z;
        static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(vel);
    }

    Vector3D& RigidBody::GetVelocity() noexcept {
        return velocity;
    }

    void RigidBody::AddForce(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->addForce(physx::PxVec3(x, y, z));
    }

    void RigidBody::AddTorque(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->addTorque(physx::PxVec3(x, y, z));
    }

    void RigidBody::SetAngularVelocity(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(physx::PxVec3(x, y, z));
    }

    void RigidBody::AddAngularVelocity(float x, float y, float z) noexcept {
        auto vel{ static_cast<physx::PxRigidDynamic*>(actor)->getAngularVelocity() };
        vel.x += x;
        vel.y += y;
        vel.z += z;
        static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(vel);
    }

    Vector3D& RigidBody::GetAngularVelocity() noexcept {
        return angularVelocity;
    }

    void RigidBody::SetHasGravity(bool hasGravity) noexcept {
        actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !hasGravity);
    }

    void RigidBody::SetTrigger(bool isTrigger) noexcept {
        this->isTrigger = isTrigger;
        if (isTrigger) {
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
            colliderType = PhysicsColliderType::eTrigger;
        } else {
            shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
            shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
            colliderType = PhysicsColliderType::eCollider;
        }
    }

    void RigidBody::SetLinearFactor(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, bool(x));
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, bool(y));
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, bool(z));
    }

    void RigidBody::SetAngularFactor(float x, float y, float z) noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, bool(x));
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, bool(y));
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, bool(z));
    }

    bool RigidBody::GetIsTrigger() const noexcept {
        return isTrigger;
    }

    void RigidBody::SetKinematic(bool isKinematic) noexcept {
        this->isKinematic = isKinematic;
        static_cast<physx::PxRigidDynamic*>(actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, isKinematic);
    }

    void RigidBody::SetGeometry(const physx::PxGeometry& geometry) {
        shape->setGeometry(geometry);
    }

    void RigidBody::ClearForces() noexcept {
        static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eFORCE);
        static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eIMPULSE);
        static_cast<physx::PxRigidDynamic*>(actor)->clearTorque();
        static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(physx::PxVec3{});
        static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(physx::PxVec3{});
        velocity        = {};
        angularVelocity = {};
    }

    void RigidBody::Destroy() noexcept {
        Clear();
    }

    void RigidBody::MoveConstruct(RigidBody&& rhs) noexcept {
        std::memcpy(this, &rhs, sizeof(*this));
        if (actor != nullptr) {
            actor->userData = this;
        }
        rhs.actor    = nullptr;
        rhs.material = nullptr;
        rhs.shape    = nullptr;
    }

    void RigidBody::Clear() noexcept {
        if (material) {
            material->release();
            material = nullptr;
        }
        if (shape) {
            shape->release();
            shape = nullptr;
        }
        if (actor) {
            scene->removeActor(*actor);
            actor->release();
            actor = nullptr;
        }
    }
} // namespace adh
