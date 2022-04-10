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
// *********************************************************************************M

#include "RigidBody.hpp"
#include <Math/Math.hpp>
// #include "Math/source/Quaternion.hpp"
// #include "PxShape.h"
// #include "foundation/PxVec3.h"
#include <btBulletDynamicsCommon.h>
#include <iostream>

#include <Event/Event.hpp>

namespace adh {
    RigidBody::RigidBody() : mActor{ nullptr } {}
    // : body{} {}

    RigidBody::~RigidBody() {
        Clear();
    }

    RigidBody::RigidBody(RigidBody&& rhs) noexcept {
        MoveConstruct(Move(rhs));
    }

    RigidBody& RigidBody::operator=(RigidBody&& rhs) noexcept {
        Clear();
        MoveConstruct(Move(rhs));

        return *this;
    }

    void RigidBody::Create(btDiscreteDynamicsWorld* world, ColliderType colliderType, std::uint64_t entity, const Transform& transform,
                           float mass, float bounciness, float friction, const Array<Vertex>& vertices) {
        // btCollisionShape* shape;
        // switch (colliderType) {
        // case ColliderType::eBox:
        //     {
        //         shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
        //         break;
        //     }
        // case ColliderType::eSphere:
        //     {
        //         shape = new btSphereShape(1.0f);
        //         break;
        //     }
        // case ColliderType::eCone:
        //     {
        //         shape = new btConeShape(1.0f, 2.0f);
        //         break;
        //     }
        // case ColliderType::eCylinder:
        //     {
        //         shape = new btCylinderShape(btVector3{ 0.5f, 0.0f, 0.0f });
        //         break;
        //     }
        // case ColliderType::eCapsule:
        //     {
        //         shape = new btCapsuleShape(1.0f, 2.0f);
        //         break;
        //     }
        // case ColliderType::eMesh:
        //     {
        //         shape = new btConvexHullShape();
        //         for (std::size_t i{}; i < vertices.GetSize(); ++i) {
        //             static_cast<btConvexHullShape*>(shape)->addPoint(
        //                 btVector3(
        //                     vertices[i].position.x,
        //                     vertices[i].position.y,
        //                     vertices[i].position.z));
        //         }
        //         break;
        //     }
        // default:; // TODO: compounds etc
        // }

        // m_World            = world;
        // this->mass         = mass;
        // this->bounciness   = bounciness;
        // this->friction     = friction;
        // this->entity       = entity;
        // this->colliderType = colliderType;

        // btTransform trans;
        // trans.setIdentity();
        // trans.setOrigin(btVector3(transform.translate.x, transform.translate.y, transform.translate.z));

        // btQuaternion q;
        // q.setEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
        // trans.setRotation(q);

        // btDefaultMotionState* motionState = new btDefaultMotionState(trans);

        // btScalar bodyMass = mass;
        // btVector3 bodyInertia;
        // if (mass > 0.0) {
        //     shape->calculateLocalInertia(bodyMass, bodyInertia);
        // }

        // btRigidBody::btRigidBodyConstructionInfo bodyCI = btRigidBody::btRigidBodyConstructionInfo(bodyMass, motionState, shape, bodyInertia);
        // // bodyCI.m_restitution                            = bounciness;
        // // bodyCI.m_friction                               = friction;

        // this->body = new btRigidBody(bodyCI);
        // body->getCollisionShape()->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
        // body->setRestitution(bounciness);
        // body->setFriction(friction);
        // body->setMassProps(bodyMass, bodyInertia);
        // body->setUserPointer(this);
        // world->addRigidBody(body);
        //
        //
        // PxShapeFlags shapeFlags = PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE;
        // shape = gPhysics->createShape(geom, *gMaterial, isExclusive, shapeFlags);

        // For this method to work, you need a way to mark shapes as triggers without using PxShapeFlag::eTRIGGER_SHAPE
        // (so that trigger-trigger pairs are reported), and without calling a PxShape function (so that the data is
        // available in a filter shader).
        //
        // One way is to reserve a special PxFilterData value/mask for triggers. It may not always be possible depending
        // on how you otherwise use the filter data).
        // const PxFilterData triggerFilterData(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);
        // shape->setSimulationFilterData(triggerFilterData);
    }

    void RigidBody::Create(Physics* world, ColliderType colliderType, std::uint64_t entity, const Transform& transform,
                           float mass, float bounciness, float friction, const Array<Vertex>& vertices) {
        using namespace physx;
        mPhysics = world;

        mMaterial = mPhysics->mPhysics->createMaterial(friction, friction, bounciness);
        mShape    = mPhysics->mPhysics->createShape(physx::PxBoxGeometry(transform.scale.x, transform.scale.y, transform.scale.z), *mMaterial, true);
        switch (colliderType) {
        case ColliderType::eBox:
            {
                break;
            }
        case ColliderType::eSphere:
            {
                break;
            }
        case ColliderType::eCone:
            {
                break;
            }
        case ColliderType::eCylinder:
            {
                break;
            }
        case ColliderType::eCapsule:
            {
                break;
            }
        case ColliderType::eMesh:
            {
                break;
            }
        default:; // TODO: compounds etc
        }

        mActor             = mPhysics->mPhysics->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
        mActor->userData   = this;
        this->mass         = mass;
        this->bounciness   = bounciness;
        this->friction     = friction;
        this->entity       = entity;
        this->colliderType = colliderType;

        // mActor->setSleepThreshold(1);
        // mActor->setKinematicTarget();

        // std::cout << mActor->getSleepThreshold() << std::endl;
        physx::PxTransform t = mActor->getGlobalPose();
        Quaternion<float> qq(transform.rotation);
        physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
        t.q = q;
        mActor->setGlobalPose(t);

        mActor->attachShape(*mShape);

        // mActor->setMass(mass);
        physx::PxRigidBodyExt::updateMassAndInertia(*mActor, mass);
        mPhysics->mScene->addActor(*mActor);
    }

    void RigidBody::OnUpdate(Transform& transform) noexcept {
        // const btTransform& transf(body->getWorldTransform());
        // const btVector3& origin(transf.getOrigin());
        // const btQuaternion& currentRotation(transf.getRotation());

        // transform.translate = Vector3D{ origin.getX(), origin.getY(), origin.getZ() };
        // // currentRotation.getEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
        // transform.q.x = currentRotation.getX();
        // transform.q.y = currentRotation.getY();
        // transform.q.z = currentRotation.getZ();
        // transform.q.w = currentRotation.getW();

        // auto vel{ body->getLinearVelocity() };
        // velocity = Vector3D{ vel.getX(), vel.getY(), vel.getZ() };
        // auto avel{ body->getAngularVelocity() };
        // angularVelocity = Vector3D{ avel.getX(), avel.getY(), avel.getZ() };

        // translate = Vector3D{ origin.getX(), origin.getY(), origin.getZ() };
        // rotation  = transform.rotation;
        // // currentRotation.getEulerZYX(rotation.z, rotation.y, rotation.x);
        // //

        physx::PxTransform t = mActor->getGlobalPose();
        transform.translate  = Vector3D{ t.p.x, t.p.y, t.p.z };
        translate            = transform.translate;
        rotation             = transform.rotation;
        transform.q.x        = t.q.x;
        transform.q.y        = t.q.y;
        transform.q.z        = t.q.z;
        transform.q.w        = t.q.w;

        auto v          = mActor->getLinearVelocity();
        velocity        = Vector3D{ v.x, v.y, v.z };
        auto av         = mActor->getAngularVelocity();
        angularVelocity = Vector3D{ av.x, av.y, av.z };
    }

    void RigidBody::SetTranslation(float x, float y, float z) noexcept {
        // btTransform& transf(body->getWorldTransform());
        // transf.setOrigin(btVector3{ x, y, z });

        physx::PxTransform t = mActor->getGlobalPose();
        t.p                  = physx::PxVec3{ x, y, z };
        mActor->setGlobalPose(t);
    }

    Vector3D& RigidBody::GetTranslation() noexcept {
        return translate;
    }

    void RigidBody::SetRotation(float x, float y, float z) noexcept {
        // btTransform& transf(body->getWorldTransform());
        // btQuaternion q(y, x, z);
        // btQuaternion q;
        // q.setEulerZYX(z, y, x);
        // transf.setRotation(q);

        {
            physx::PxTransform t = mActor->getGlobalPose();
            Quaternion<float> qq(Vector3D{ x, y, z });
            physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
            t.q = q;
            mActor->setGlobalPose(t);
        }
    }

    void RigidBody::AddRotation(float x, float y, float z) noexcept {
        // btTransform& transf(body->getWorldTransform());
        // btQuaternion q(y, x, z);
        // btQuaternion q;
        // q.setEulerZYX(z, y, x);
        // transf.setRotation(q);

        {
            physx::PxTransform t = mActor->getGlobalPose();
            Quaternion<float> qq(Vector3D{ x, y, z });
            physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
            t.q *= q;
            mActor->setGlobalPose(t);
        }
    }

    Vector3D& RigidBody::GetRotation() noexcept {
        return rotation;
    }

    void RigidBody::SetVelocity(float x, float y, float z) noexcept {
        // body->setLinearVelocity(btVector3{ x, y, z });
        mActor->setLinearVelocity(physx::PxVec3(x, y, z));
    }

    void RigidBody::AddVelocity(float x, float y, float z) noexcept {
        // auto vel{ body->getLinearVelocity() };
        // vel.setX(vel.getX() + x);
        // vel.setY(vel.getY() + y);
        // vel.setZ(vel.getZ() + z);
        // body->setLinearVelocity(vel);
        //

        auto vel{ mActor->getLinearVelocity() };
        vel.x += x;
        vel.y += y;
        vel.z += z;
        mActor->setLinearVelocity(vel);
    }

    Vector3D& RigidBody::GetVelocity() noexcept {
        return velocity;
    }

    void RigidBody::SetAngularVelocity(float x, float y, float z) noexcept {
        // body->setAngularVelocity(btVector3{ x, y, z });
        mActor->setAngularVelocity(physx::PxVec3(x, y, z));
    }

    void RigidBody::AddAngularVelocity(float x, float y, float z) noexcept {
        // auto vel{ body->getAngularVelocity() };
        // vel.setX(vel.getX() + x);
        // vel.setY(vel.getY() + y);
        // vel.setZ(vel.getZ() + z);
        // body->setAngularVelocity(vel);

        auto vel{ mActor->getAngularVelocity() };
        vel.x += x;
        vel.y += y;
        vel.z += z;
        mActor->setAngularVelocity(vel);
    }

    Vector3D& RigidBody::GetAngularVelocity() noexcept {
        return angularVelocity;
    }

    void RigidBody::SetGravity(float x, float y, float z) noexcept {
        // body->setGravity(btVector3{ x, y, z });
        mActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    }

    void RigidBody::SetBodyType(BodyType collisionType) noexcept {
        // switch (collisionType) {
        // case BodyType::eStatic:
        //     {
        //         body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        //         break;
        //     }
        // case BodyType::eKinematic:
        //     {
        //         body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        //         break;
        //     }
        // case BodyType::eDynamic:
        //     {
        //         body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
        //         break;
        //     }
        // }
    }

    void RigidBody::SetIsTrigger(bool isTrigger) noexcept {
        // this->isTrigger = isTrigger;
        // if (isTrigger) {
        //     body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        // } else {
        //     auto bit{ 1u };
        //     auto flag = btCollisionObject::CF_NO_CONTACT_RESPONSE;
        //     body->setCollisionFlags(body->getCollisionFlags() & ~(bit << flag));
        // }

        this->isTrigger = isTrigger;
        // mShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
        // mShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, isTrigger);
        // mShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
        // mShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, isTrigger);
        // mActor->attachShape(*mShape);
    }

    void RigidBody::SetLinearFactor(float x, float y, float z) noexcept {
        // body->setLinearFactor(btVector3(x, y, z));
        using namespace physx;
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, bool(x));
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, bool(y));
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, bool(z));
    }

    void RigidBody::SetAngularFactor(float x, float y, float z) noexcept {
        // body->setAngularFactor(btVector3(x, y, z));
        using namespace physx;
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, bool(x));
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, bool(y));
        mActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, bool(z));
    }

    bool RigidBody::GetIsTrigger() const noexcept {
        return isTrigger;
    }

    void RigidBody::ClearGravity() noexcept {
        // body->clearGravity();
        // mActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    }

    void RigidBody::ClearForces() noexcept {
        // body->clearForces();
        mActor->clearForce(physx::PxForceMode::eFORCE);
        mActor->clearForce(physx::PxForceMode::eIMPULSE);
        mActor->clearTorque();
    }

    void RigidBody::Destroy() noexcept {
        Clear();
    }

    void RigidBody::MoveConstruct(RigidBody&& rhs) noexcept {
        // body = rhs.body;
        // if (body != nullptr) {
        //     body->setUserPointer(this);
        // }
        // m_World         = rhs.m_World;
        // mass            = rhs.mass;
        // bounciness      = rhs.bounciness;
        // friction        = rhs.friction;
        // entity          = rhs.entity;
        // isTrigger       = rhs.isTrigger;
        // colliderType    = rhs.colliderType;
        // velocity        = rhs.velocity;
        // angularVelocity = rhs.angularVelocity;
        // rhs.body        = nullptr;
        // rhs.m_World     = nullptr;

        mActor = rhs.mActor;
        if (mActor != nullptr) {
            mActor->userData = this;
        }

        mMaterial       = rhs.mMaterial;
        mShape          = rhs.mShape;
        mPhysics        = rhs.mPhysics;
        mass            = rhs.mass;
        bounciness      = rhs.bounciness;
        friction        = rhs.friction;
        entity          = rhs.entity;
        isTrigger       = rhs.isTrigger;
        isKinematic     = rhs.isKinematic;
        colliderType    = rhs.colliderType;
        velocity        = rhs.velocity;
        angularVelocity = rhs.angularVelocity;
        rhs.mActor      = nullptr;
        rhs.mMaterial   = nullptr;
        rhs.mShape      = nullptr;
        rhs.mPhysics    = nullptr;
    }

    void RigidBody::Clear() noexcept {
        // if (body) {
        //     delete body->getMotionState();
        //     delete body->getCollisionShape();
        //     m_World->removeRigidBody(body);
        //     delete body;
        //     // count--;
        // }

        if (mActor) {
            mActor->release();
            mMaterial->release();
            mShape->release();
        }
    }
} // namespace adh
