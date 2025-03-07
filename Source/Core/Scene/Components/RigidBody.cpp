#include "RigidBody.hpp"
#include "foundation/PxVec3.h"
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
                           float staticFriction,
                           float dynamicFriction,
                           float restitution,
                           PhysicsBodyType bodyType,
                           float mass,
                           bool isKinematic,
                           bool isTrigger,
                           bool scaleSameAsModel,
                           PhysicsColliderShape colliderShape,
                           PhysicsColliderType colliderType,
                           const Vector3D& scale,
                           float radius,
                           float halfHeight,
                           const Mesh* const mesh) {
        this->entity           = entity;
        this->scaleSameAsModel = scaleSameAsModel;

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
        physx::PxFilterData filter;
        filter.word0 = 1;
        filter.word1 = 1;
        shape->setSimulationFilterData(filter);

        actor->userData = this;
    }

    void RigidBody::OnUpdate(Transform& transform) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        if (t.isValid() && t.isSane()) {
            transform.translate = Vector3D{ t.p.x, t.p.y, t.p.z };
            transform.q.x       = t.q.x;
            transform.q.y       = t.q.y;
            transform.q.z       = t.q.z;
            transform.q.w       = t.q.w;

            translate = transform.translate;
            rotation  = transform.rotation;

            if (bodyType == PhysicsBodyType::eDynamic) {
                auto v          = static_cast<physx::PxRigidDynamic*>(actor)->getLinearVelocity();
                velocity        = Vector3D{ v.x, v.y, v.z };
                auto av         = static_cast<physx::PxRigidDynamic*>(actor)->getAngularVelocity();
                angularVelocity = Vector3D{ av.x, av.y, av.z };
            }
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

    void RigidBody::SetPosition(float x, float y, float z) noexcept {
        physx::PxTransform t = actor->getGlobalPose();
        physx::PxVec3 p(x, y, z);
        t.p = p;
        actor->setGlobalPose(t);
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
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(physx::PxVec3(x, y, z));
        }
    }

    void RigidBody::AddVelocity(float x, float y, float z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            auto vel{ static_cast<physx::PxRigidDynamic*>(actor)->getLinearVelocity() };
            vel.x += x;
            vel.y += y;
            vel.z += z;
            static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(vel);
        }
    }

    Vector3D& RigidBody::GetVelocity() noexcept {
        return velocity;
    }

    void RigidBody::AddForce(float x, float y, float z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->addForce(physx::PxVec3(x, y, z));
        }
    }

    void RigidBody::AddTorque(float x, float y, float z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->addTorque(physx::PxVec3(x, y, z));
        }
    }

    float RigidBody::GetRestitution() const noexcept {
        return restitution;
    }

    void RigidBody::SetRestitution(float restitution) noexcept {
        this->restitution = restitution;
        material->setRestitution(restitution);
    }

    float RigidBody::GetStaticFriction() const noexcept {
        return staticFriction;
    }

    void RigidBody::SetStaticFriction(float staticFriction) noexcept {
        this->staticFriction = staticFriction;
        material->setStaticFriction(staticFriction);
    }

    float RigidBody::GetDynamicFriction() const noexcept {
        return dynamicFriction;
    }

    void RigidBody::SetDynamicFriction(float dynamicFriction) noexcept {
        this->dynamicFriction = dynamicFriction;
        material->setDynamicFriction(dynamicFriction);
    }

    float RigidBody::GetMass() const noexcept {
        return mass;
    }

    void RigidBody::SetMass(float mass) noexcept {
        this->mass = mass;
        physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(actor), mass);
    }

    void RigidBody::SetAngularVelocity(float x, float y, float z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(physx::PxVec3(x, y, z));
        }
    }

    void RigidBody::AddAngularVelocity(float x, float y, float z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            auto vel{ static_cast<physx::PxRigidDynamic*>(actor)->getAngularVelocity() };
            vel.x += x;
            vel.y += y;
            vel.z += z;
            static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(vel);
        }
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

    void RigidBody::SetLinearFactor(bool x, bool y, bool z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, x);
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, y);
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, z);
        }
    }

    void RigidBody::SetAngularFactor(bool x, bool y, bool z) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, x);
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, y);
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, z);
        }
    }

    bool RigidBody::GetIsTrigger() const noexcept {
        return isTrigger;
    }

    void RigidBody::SetKinematic(bool isKinematic) noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            this->isKinematic = isKinematic;
            static_cast<physx::PxRigidDynamic*>(actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, isKinematic);
        }
    }

    void RigidBody::SetGeometry(const physx::PxGeometry& geometry) {
        shape->setGeometry(geometry);
    }

    void RigidBody::UpdateGeometry() noexcept {
        if (colliderShape == PhysicsColliderShape::eBox) {
            SetGeometry(physx::PxBoxGeometry{ scale.x, scale.y, scale.z });
        } else if (colliderShape == PhysicsColliderShape::eSphere) {
            SetGeometry(physx::PxSphereGeometry{ radius });
        } else if (colliderShape == PhysicsColliderShape::eCapsule) {
            SetGeometry(physx::PxCapsuleGeometry{ radius, halfHeight });

        } else if (colliderShape == PhysicsColliderShape::eMesh) {
            physx::PxTriangleMeshGeometry geometry2;
            shape->getTriangleMeshGeometry(geometry2);
            physx::PxMeshScale s(physx::PxVec3(scale[0], scale[1], scale[2]), physx::PxQuat(physx::PxIdentity));
            shape->setGeometry(physx::PxTriangleMeshGeometry(geometry2.triangleMesh, s));
            // SetGeometry(physx::PxTriangleMeshGeometry(geometry2.triangleMesh, s));

        } else if (colliderShape == PhysicsColliderShape::eConvexMesh) {
            physx::PxConvexMeshGeometry geometry2;
            shape->getConvexMeshGeometry(geometry2);
            physx::PxMeshScale s(physx::PxVec3(scale[0], scale[1], scale[2]), physx::PxQuat(physx::PxIdentity));
            shape->setGeometry(physx::PxConvexMeshGeometry(geometry2.convexMesh, s));
            // SetGeometry(physx::PxConvexMeshGeometry(geometry2.convexMesh, s));
        }
    }

    void RigidBody::ClearForces() noexcept {
        if (bodyType == PhysicsBodyType::eDynamic) {
            if (!isKinematic) {
                static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eFORCE);
                static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eIMPULSE);
                static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eACCELERATION);
                static_cast<physx::PxRigidDynamic*>(actor)->clearForce(physx::PxForceMode::eVELOCITY_CHANGE);
                static_cast<physx::PxRigidDynamic*>(actor)->clearTorque();
                static_cast<physx::PxRigidDynamic*>(actor)->setLinearVelocity(physx::PxVec3{ 0.0f, 0.0f, 0.0f }, false);
                static_cast<physx::PxRigidDynamic*>(actor)->setAngularVelocity(physx::PxVec3{ 0.0f, 0.0f, 0.0f }, false);
                velocity        = {};
                angularVelocity = {};
            }
        }
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
        if (actor) {
            actor->getScene()->removeActor(*actor);
            actor->release(); // NOTE: also releases shape
            shape = nullptr;
            actor = nullptr;
        }
    }
} // namespace adh
