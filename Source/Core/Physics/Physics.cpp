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

#include "Physics.hpp"

#include <Std/Utility.hpp>

#include <Event/Event.hpp>

#include <Scene/Components/RigidBody.hpp>

namespace adh {
    static void TickCollisionCallback(btDynamicsWorld* world, btScalar timeStep) {
        std::int32_t numManifolds{ world->getDispatcher()->getNumManifolds() };
        for (std::int32_t i{}; i < numManifolds; ++i) {
            btPersistentManifold* contactManifold{ world->getDispatcher()->getManifoldByIndexInternal(i) };

            std::int32_t numContacts{ contactManifold->getNumContacts() };
            if (numContacts > 0) {
                const btCollisionObject* obA{ contactManifold->getBody0() };
                const btCollisionObject* obB{ contactManifold->getBody1() };

                auto bA{ (static_cast<RigidBody*>(obA->getUserPointer())) };
                auto bB{ (static_cast<RigidBody*>(obB->getUserPointer())) };

                auto entityA = bA->entity;
                auto entityB = bB->entity;

                ADH_THROW(entityA != 0, "Collision callback returned null entity!");
                ADH_THROW(entityB != 0, "Collision callback returned null entity!");

                if (!obA->hasContactResponse() || !obB->hasContactResponse()) {
                    Event::Dispatch<CollisionEvent>(CollisionEvent::Type::eTrigger, entityA, entityB);
                } else {
                    Event::Dispatch<CollisionEvent>(CollisionEvent::Type::eCollision, entityA, entityB);
                }
            }
        }
    }

    Physics::Physics()
        : m_Broadphase{},
          m_CollisionConfiguration{},
          m_Dispatcher{},
          m_Solver{},
          m_World{} {}

    Physics::Physics(Physics&& rhs) noexcept {
        MoveConstruct(Move(rhs));
    }

    Physics& Physics::operator=(Physics&& rhs) noexcept {
        Clear();
        MoveConstruct(Move(rhs));

        return *this;
    }

    Physics::~Physics() {
        Clear();
    }

    void Physics::Create() {
        m_Broadphase             = new btDbvtBroadphase();
        m_CollisionConfiguration = new btDefaultCollisionConfiguration();
        m_Dispatcher             = new btCollisionDispatcher(m_CollisionConfiguration);
        m_Solver                 = new btSequentialImpulseConstraintSolver();
        m_World                  = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

        m_World->setGravity(m_Gravity);
        m_World->setInternalTickCallback(TickCollisionCallback);
    }

    void Physics::Destroy() {
        Clear();
    }

    void Physics::OnUpdate(float deltaTime) {
        StepSimulation(deltaTime);
    }

    void Physics::StepSimulation(float deltaTime) {
        m_World->stepSimulation(deltaTime, deltaTime);
    }

    void Physics::SetGravity(const Vector3D& gravity) noexcept {
        m_World->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
    }

    void Physics::ClearForces() noexcept {
        m_World->clearForces();
    }

    void Physics::MoveConstruct(Physics&& rhs) noexcept {
        m_Broadphase                 = rhs.m_Broadphase;
        m_CollisionConfiguration     = rhs.m_CollisionConfiguration;
        m_Dispatcher                 = rhs.m_Dispatcher;
        m_Solver                     = rhs.m_Solver;
        m_World                      = rhs.m_World;
        rhs.m_Broadphase             = nullptr;
        rhs.m_CollisionConfiguration = nullptr;
        rhs.m_Dispatcher             = nullptr;
        rhs.m_Solver                 = nullptr;
        rhs.m_World                  = nullptr;
    }

    void Physics::Clear() noexcept {
        if (m_World) {
            m_World->setInternalTickCallback(nullptr);
            delete m_World;
            m_World = nullptr;
        }
        if (m_Solver) {
            delete m_Solver;
            m_Solver = nullptr;
        }
        if (m_Dispatcher) {
            delete m_Dispatcher;
            m_Dispatcher = nullptr;
        }
        if (m_CollisionConfiguration) {
            delete m_CollisionConfiguration;
            m_CollisionConfiguration = nullptr;
        }
        if (m_Broadphase) {
            delete m_Broadphase;
            m_Broadphase = nullptr;
        }
    }
} // namespace adh
