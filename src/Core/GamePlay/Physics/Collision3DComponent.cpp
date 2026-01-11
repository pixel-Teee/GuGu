#include <pch.h>

#include "Collision3DComponent.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/TransformComponent.h>
#include <btBulletDynamicsCommon.h>

#include <Core/Physics/PhysicsManager.h>

namespace GuGu {
	static bool registerGuGuCollision3DComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::Collision3DComponent");
		auto& type = db.types[id];
		meta::TypeInfo<Collision3DComponent>::Register(id, type, true, "697F186C-FB1C-43E8-9FE2-8A468A7936BE");

		auto typeID = typeidof(Collision3DComponent);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<Collision3DComponent>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<Collision3DComponent>();

			type.AddConstructor<Collision3DComponent, false, false>({});

			type.AddConstructor<Collision3DComponent, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(Component) });

			meta::TypeInfo<Collision3DComponent>::Defined = true;

			// 			type.AddField<Collision3DComponent, float>("m_fov",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getFov,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setFov, {});
			// 
			// 			type.AddField<Collision3DComponent, float>("m_nearPlane",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getNearPlane,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setNearPlane, {});
			// 
			// 			type.AddField<Collision3DComponent, float>("m_farPlane",
			// 				(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getFarPlane,
			// 				(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setFarPlane, {});
			// 
			// 			type.AddField<Collision3DComponent, std::weak_ptr<GameObject>>("m_owner",
			// 				(meta::FieldGetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::getParentGameObject,
			// 				(meta::FieldSetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::setParentGameObject, {});
		}

		{
			auto id = db.AllocateType("std::shared_ptr<Collision3DComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<Collision3DComponent>>::Register(id, type, false, "092FBE34-35E6-42B0-B5E5-77A350673442");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<Collision3DComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<Collision3DComponent>>::Register(id, type, false, "2ECFEF8E-C22F-45AE-8F27-E213B8CF3627");
		}

		{
			auto id = db.AllocateType("GuGu::CollisionShape");
			auto& type = db.types[id];

			meta::TypeInfo<GuGu::CollisionShape>::Register(id, type, true, "88ECF076-3D0C-4CF8-9B75-F94B36D2F7D6");

			auto typeID = typeidof(GuGu::CollisionShape);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GuGu::CollisionShape>::Defined)
			{
				auto& type = db.types[typeID];

				type.SetEnum<GuGu::CollisionShape>("GuGu::CollisionShape", {
					{"Box", CollisionShape::Box},
					{"Sphere", CollisionShape::Sphere},
					{"Capsule", CollisionShape::Capsule}
				});
			}
		}

		return true;
	}

	static bool registerGuGuCollision3DComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(Collision3DComponent).GetID()];

		type.AddField<Collision3DComponent, CollisionShape>("m_shape",
			(meta::FieldGetter<Collision3DComponent, CollisionShape, true>::Signature) & Collision3DComponent::getShape,
			(meta::FieldSetter<Collision3DComponent, CollisionShape, true>::Signature) & Collision3DComponent::setShape, {});
		type.AddField<Collision3DComponent, math::float3>("m_boxHalfExtents",
			(meta::FieldGetter<Collision3DComponent, math::float3&, true>::Signature) & Collision3DComponent::getBoxHalfExtents,
			(meta::FieldSetter<Collision3DComponent, math::float3, true>::Signature) & Collision3DComponent::setBoxHalfExtents, {});
		type.AddField<Collision3DComponent, float>("m_sphereRadius",
			(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getSphereRadius,
			(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setSphereRadius, {});
		type.AddField<Collision3DComponent, float>("m_capsuleRadius",
			(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getCapsuleRadius,
			(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setCapsuleRadius, {});
		type.AddField<Collision3DComponent, float>("m_capsuleHeight",
			(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getCapsuleHeight,
			(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setCapsuleHeight, {});
		type.AddField<Collision3DComponent, float>("m_mass",
			(meta::FieldGetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::getMass,
			(meta::FieldSetter<Collision3DComponent, float, true>::Signature) & Collision3DComponent::setMass, {});
		type.AddField<Collision3DComponent, float>("m_friction",
			(meta::FieldGetter<Collision3DComponent, float, false>::Signature) & Collision3DComponent::m_friction,
			(meta::FieldSetter<Collision3DComponent, float, false>::Signature) & Collision3DComponent::m_friction, {});
		type.AddField<Collision3DComponent, float>("m_restitution",
			(meta::FieldGetter<Collision3DComponent, float, false>::Signature) & Collision3DComponent::m_restitution,
			(meta::FieldSetter<Collision3DComponent, float, false>::Signature) & Collision3DComponent::m_restitution, {});

		type.AddField<Collision3DComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::getParentGameObject,
			(meta::FieldSetter<Collision3DComponent, std::weak_ptr<GameObject>&, true>::Signature) & Collision3DComponent::setParentGameObject, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(Collision3DComponent)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuCollision3DComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(Collision3DComponent)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuCollision3DComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	Collision3DComponent::Collision3DComponent()
	{
		m_boxHalfExtents = math::float3(0.5f, 0.5f, 0.5f);
		m_sphereRadius = 0.5f;
		m_capsuleRadius = 0.5f;
		m_capsuleHeight = 0.5f;

		//质量
		m_mass = 1.0f;

		//摩擦力
		m_friction = 1.0f;

		//弹性系数
		m_restitution = 1.0f;

		//default shape
		m_shape = CollisionShape::Box;

		//recreateBulletShape();
	}

	Collision3DComponent::~Collision3DComponent()
	{

	}

	meta::Object* Collision3DComponent::Clone(void) const
	{
		Collision3DComponent* collision3d = new Collision3DComponent();
		//nothing
		return collision3d;
	}

	void Collision3DComponent::Update(float fElapsedTimeSeconds)
	{
		if (m_shape == CollisionShape::Box)
		{
			std::shared_ptr<btBoxShape> boxShape = std::static_pointer_cast<btBoxShape>(m_collisionShape);
			btVector3 halfExtents = boxShape->getHalfExtentsWithoutMargin();
			math::float3 halfExtents2(halfExtents.x(), halfExtents.y(), halfExtents.z());
			math::bool3 isEqual = math::isnear(m_boxHalfExtents, halfExtents2);
			if (!math::all(isEqual))
			{
				//update shape
				boxShape->setImplicitShapeDimensions(btVector3(m_boxHalfExtents.x, m_boxHalfExtents.y, m_boxHalfExtents.z));
				std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
				if (physicsWorld)
				{
					physicsWorld->updateAabbs();
				}
			}
		}
		else if (m_shape == CollisionShape::Capsule)
		{

		}
		else if (m_shape == CollisionShape::Sphere)
		{
			std::shared_ptr<btSphereShape> sphereShape = std::static_pointer_cast<btSphereShape>(m_collisionShape);
			float radius = sphereShape->getRadius();
			if (!math::isnear(m_sphereRadius, radius))
			{
				sphereShape->setUnscaledRadius(m_sphereRadius);
				std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
				if (physicsWorld)
				{
					physicsWorld->updateAabbs();
				}
			}		
		}	
	}

	meta::Type Collision3DComponent::GetType() const
	{
		return typeof(Collision3DComponent);
	}

	CollisionShape Collision3DComponent::getShape() const
	{
		return m_shape;
	}

	void Collision3DComponent::setShape(CollisionShape inCollisionShape)
	{
		m_shape = inCollisionShape;
		recreateBulletShape();
	}

	void Collision3DComponent::recreateBulletShape()
	{
		std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
		if (physicsWorld)
		{
			//移除原先的刚体
			if (m_rigidBody)
			{
				physicsWorld->removeRigidBody(m_rigidBody.get());
				m_rigidBody = nullptr;
			}
		}
		//创建新的形状和刚体
		if (m_shape == CollisionShape::Box)
		{
			m_collisionShape = std::make_shared<btBoxShape>(btVector3(m_boxHalfExtents[0], m_boxHalfExtents[1], m_boxHalfExtents[2]));
		}
		else if (m_shape == CollisionShape::Sphere)
		{
			m_collisionShape = std::make_shared<btSphereShape>(m_sphereRadius);
		}
		else if (m_shape == CollisionShape::Capsule)
		{

		}
		//惯性张量
		btVector3 localInertia(0, 0, 0);
		if (m_mass != 0.0f)
		{
			m_collisionShape->calculateLocalInertia(m_mass, localInertia);
		}

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(0, 0, 0));

		m_motionState = std::make_shared<btDefaultMotionState>(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, m_motionState.get(), m_collisionShape.get(), localInertia);
		m_rigidBody = std::make_shared<btRigidBody>(rbInfo);

		if (physicsWorld)
		{
			//添加新的刚体
			physicsWorld->addRigidBody(m_rigidBody.get());
		}
	}

	void Collision3DComponent::setBoxHalfExtents(math::float3 inBoxHalfExtents)
	{
		m_boxHalfExtents = inBoxHalfExtents;
		std::shared_ptr<btBoxShape> boxShape = std::static_pointer_cast<btBoxShape>(m_collisionShape);
		boxShape->setImplicitShapeDimensions(btVector3(m_boxHalfExtents.x, m_boxHalfExtents.y, m_boxHalfExtents.z));

		std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
		if (physicsWorld)
		{
			physicsWorld->updateAabbs();
		}
	}

	math::float3& Collision3DComponent::getBoxHalfExtents()
	{
		return m_boxHalfExtents;
	}

	void Collision3DComponent::setSphereRadius(float sphereRadius)
	{
		m_sphereRadius = sphereRadius;
		std::shared_ptr<btSphereShape> sphereShape = std::static_pointer_cast<btSphereShape>(m_collisionShape);
		sphereShape->setUnscaledRadius(m_sphereRadius);

		std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
		if (physicsWorld)
		{
			physicsWorld->updateAabbs();
		}
	}

	float Collision3DComponent::getSphereRadius() const
	{
		return m_sphereRadius;
	}

	void Collision3DComponent::setCapsuleRadius(float capsuleRadius)
	{
		m_capsuleRadius = capsuleRadius;
	}

	float Collision3DComponent::getCapsuleRadius() const
	{
		return m_capsuleRadius;
	}

	void Collision3DComponent::setCapsuleHeight(float inHeight)
	{
		m_capsuleHeight = inHeight;
	}

	float Collision3DComponent::getCapsuleHeight() const
	{
		return m_capsuleHeight;
	}

	void Collision3DComponent::setMass(float newMass)
	{
		float oldMass = m_mass;
		bool isStatic = (oldMass == 0.0f);
		bool willBeStatic = (newMass == 0.0f);

		//静态与动态发生变化
		if (isStatic != willBeStatic)
		{
			std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
			if (physicsWorld)
			{
				if(m_rigidBody)
					physicsWorld->removeRigidBody(m_rigidBody.get());

				if (m_rigidBody)
				{
					//更新碰撞标志
					int32_t flags = m_rigidBody->getCollisionFlags();
					if (willBeStatic)
					{
						flags |= btCollisionObject::CF_STATIC_OBJECT;
						flags &= ~btCollisionObject::CF_DYNAMIC_OBJECT;
					}
					else
					{
						flags |= btCollisionObject::CF_DYNAMIC_OBJECT;
						flags &= ~btCollisionObject::CF_STATIC_OBJECT;
					}
					m_rigidBody->setCollisionFlags(flags);
				}

				//更新质量和惯性
				updateMassAndInertia(newMass);

				// 重新添加回物理世界（可能需要更新碰撞过滤组）
				// short group = willBeStatic ? COLLISION_GROUP_STATIC : COLLISION_GROUP_DYNAMIC;
				// short mask = willBeStatic ? COLLISION_MASK_STATIC : COLLISION_MASK_DYNAMIC;
				if(m_rigidBody)
					physicsWorld->addRigidBody(m_rigidBody.get());
			}
		}
		else if (!willBeStatic) //动态到动态，仅仅只改变质量
		{
			updateMassAndInertia(newMass);
		}

		m_mass = newMass;
	}

	float Collision3DComponent::getMass() const
	{
		return m_mass;
	}

	void Collision3DComponent::updateMassAndInertia(float newMass)
	{
		btVector3 localInertia(0, 0, 0);

		if (newMass > 0.0f) {
			if (m_rigidBody)
			{
				//对于动态物体，根据形状重新计算惯性
				m_rigidBody->getCollisionShape()->calculateLocalInertia(newMass, localInertia);
			}	
		}
		//对于静态物体（newMass == 0），惯性保持为零

		if (m_rigidBody)
		{
			//这是更新质量和惯性的核心Bullet API
			m_rigidBody->setMassProps(newMass, localInertia);

			//更新内部缓存的质心变换（重要步骤，确保后续计算正确）
			m_rigidBody->updateInertiaTensor();
		}
	}

	void Collision3DComponent::syncFromPhysics()
	{
		if (!m_rigidBody || !m_owner.lock()) return;

		btTransform transform;
		m_rigidBody->getMotionState()->getWorldTransform(transform);

		//转换Bullet变换到引擎的坐标系
		btVector3 pos = transform.getOrigin();
		btQuaternion rot = transform.getRotation();

		//同步到游戏对象
		if (m_owner.lock())
		{
			std::shared_ptr<GameObject> ownerGameObj = m_owner.lock();
			std::shared_ptr<TransformComponent> trans = ownerGameObj->getComponent<TransformComponent>();
			if (trans)
			{
				trans->SetTranslation(math::double3(pos.x(), pos.y(), pos.z()));
				trans->SetRotationQuat(math::dquat(rot.x(), rot.y(), rot.z(), rot.w()));
			}
		}
	}

	void Collision3DComponent::syncToPhysics() {
		if (!m_rigidBody || !m_owner.lock()) return;

		math::double3 translation;
		math::dquat rot;

		if (m_owner.lock())
		{
			std::shared_ptr<GameObject> ownerGameObj = m_owner.lock();
			std::shared_ptr<TransformComponent> trans = ownerGameObj->getComponent<TransformComponent>();
			if (trans)
			{
				translation = trans->getTranslation();
				rot = trans->getRotationQuat();
			}
		}
		
		btTransform transform;
		transform.setOrigin(btVector3(translation.x, translation.y, translation.z));
		transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		m_rigidBody->setWorldTransform(transform);
		m_rigidBody->getMotionState()->setWorldTransform(transform);
	}

	void Collision3DComponent::PostLoad()
	{
		recreateBulletShape();
	}

	void Collision3DComponent::removeRigidBodyFromPhysics()
	{
		std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
		if (m_rigidBody && isRigidBodyInPhysics())
			physicsWorld->removeRigidBody(m_rigidBody.get());
	}

	void Collision3DComponent::addRigidBodyToPhysics()
	{
		std::shared_ptr<btDynamicsWorld> physicsWorld = PhysicsManager::getPhysicsManager().getDynamicsWorld();
		if (m_rigidBody && !isRigidBodyInPhysics())
			physicsWorld->addRigidBody(m_rigidBody.get());
	}

	bool Collision3DComponent::isRigidBodyInPhysics()
	{
		if (m_rigidBody && m_rigidBody->getBroadphaseHandle() != nullptr)
		{
			return true;
		}
		return false;
	}

}

