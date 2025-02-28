#include "Scene.hpp"
#include <Math/Math.hpp>

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

    PhysicsWorld& Scene::GetPhysics() {
        return m_PhysicsWorld;
    }

    const PhysicsWorld& Scene::GetPhysics() const {
        return m_PhysicsWorld;
    }

    void Scene::ResetPhysicsWorld() {
        m_World.GetSystem<Transform, RigidBody>().ForEach([&](Transform& transform, RigidBody& rigidBody) {
            if (rigidBody.bodyType == PhysicsBodyType::eDynamic) {
                rigidBody.ClearForces();
                physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(rigidBody.actor), rigidBody.mass);
                static_cast<physx::PxRigidDynamic*>(rigidBody.actor)->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, rigidBody.isKinematic);
                rigidBody.SetKinematic(rigidBody.isKinematic);
            }

            rigidBody.material->setStaticFriction(rigidBody.staticFriction);
            rigidBody.material->setDynamicFriction(rigidBody.dynamicFriction);
            rigidBody.material->setRestitution(rigidBody.restitution);

            if (rigidBody.colliderShape == PhysicsColliderShape::eBox) {
                if (rigidBody.scaleSameAsModel) {
                    rigidBody.SetGeometry(physx::PxBoxGeometry{ transform.scale.x, transform.scale.y, transform.scale.z });
                } else {
                    rigidBody.SetGeometry(physx::PxBoxGeometry{ rigidBody.scale.x, rigidBody.scale.y, rigidBody.scale.z });
                }
            } else if (rigidBody.colliderShape == PhysicsColliderShape::eSphere) {
                rigidBody.radius = (transform.scale[0] + transform.scale[1] + transform.scale[2]) / 3;
                rigidBody.SetGeometry(physx::PxSphereGeometry{ rigidBody.radius });
            } else if (rigidBody.colliderShape == PhysicsColliderShape::eCapsule) {
                rigidBody.radius = (transform.scale[0] + transform.scale[1] + transform.scale[2]) / 3;
                rigidBody.SetGeometry(physx::PxCapsuleGeometry{ rigidBody.radius, rigidBody.halfHeight });
            } else if (rigidBody.colliderShape == PhysicsColliderShape::eMesh || rigidBody.colliderShape == PhysicsColliderShape::eConvexMesh) {
                rigidBody.scale = transform.scale;
                rigidBody.UpdateGeometry();
            }
            rigidBody.SetTrigger(rigidBody.isTrigger);

            physx::PxTransform t;
            t.p = physx::PxVec3{ transform.translate.x, transform.translate.y, transform.translate.z };
            Quaternion<float> qq(transform.rotation);
            physx::PxQuat q(qq.x, qq.y, qq.z, qq.w);
            t.q = q;
            rigidBody.actor->setGlobalPose(t);
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
