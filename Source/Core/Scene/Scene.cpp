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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// *********************************************************************************

#include "Scene.hpp"

namespace adh {
    Scene::Scene(std::string tag)
        : m_Tag{ Move(tag) },
          m_State{ lua::NewState() },
          m_Serializer(this) {
        m_PhysicsWorld.Create();
    }

    ecs::World& Scene::GetWorld() {
        return m_World;
    }

    const ecs::World& Scene::GetWorld() const {
        return m_World;
    }

    lua::State& Scene::GetState() {
        return m_State;
    }

    const lua::State& Scene::GetState() const {
        return m_State;
    }

    Physics& Scene::GetPhysics() {
        return m_PhysicsWorld;
    }

    const Physics& Scene::GetPhysics() const {
        return m_PhysicsWorld;
    }

    void Scene::ResetPhysicsWorld() {
        m_PhysicsWorld.m_World->clearForces();
        m_World.GetSystem<Transform, RigidBody>().ForEach([&](Transform& transform, RigidBody& rigidBody) {
            // Reset forces and velocity
            rigidBody.body->clearGravity();
            rigidBody.body->clearForces();
            btVector3 zeroVector(0, 0, 0);
            rigidBody.body->setLinearVelocity(zeroVector);
            rigidBody.body->setAngularVelocity(zeroVector);
            rigidBody.body->setActivationState(DISABLE_DEACTIVATION);
            rigidBody.velocity        = {};
            rigidBody.angularVelocity = {};

            // Update collision flags
            rigidBody.body->updateInertiaTensor();
            btScalar bodyMass = rigidBody.mass;
            btVector3 bodyInertia{};
            if (rigidBody.mass > 0.0f) {
                rigidBody.body->setCollisionFlags(rigidBody.body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
                rigidBody.body->getCollisionShape()->calculateLocalInertia(bodyMass, bodyInertia);
            } else {
                rigidBody.body->setCollisionFlags(rigidBody.body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
            }

            // Update mass, friciton and bounciness
            rigidBody.body->setMassProps(bodyMass, bodyInertia);
            rigidBody.body->setFriction(rigidBody.friction);
            rigidBody.body->setRestitution(rigidBody.bounciness);

            // Update scale
            rigidBody.body->getCollisionShape()->setLocalScaling(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
            m_PhysicsWorld.m_World->getCollisionWorld()->updateSingleAabb(rigidBody.body);

            // Update position and rotation
            btTransform& trans = rigidBody.body->getWorldTransform();
            trans.setOrigin(btVector3(transform.translate.x, transform.translate.y, transform.translate.z));

            btQuaternion q(transform.rotation.y, transform.rotation.x, transform.rotation.z);
            // btQuaternion q;
            // q.setEulerZYX(transform.rotation.z, transform.rotation.y, transform.rotation.x);
            trans.setRotation(q);

            rigidBody.body->setWorldTransform(trans);
        });
    }

    const std::string& Scene::GetTag() const noexcept {
        return m_Tag;
    }

    void Scene::SetTag(std::string newTag) {
        m_Tag = Move(newTag);
    }

    void Scene::Save() {
        m_Serializer.Serialize();
    }

    void Scene::SaveToFile(const char* filePath) {
        m_Serializer.SerializeToFile(filePath);
    }

    void Scene::Load() {
        m_Serializer.Deserialize();
    }

    void Scene::LoadFromFile(const char* filePath) {
        m_Serializer.DeserializeFromFile(filePath);
    }
} // namespace adh
