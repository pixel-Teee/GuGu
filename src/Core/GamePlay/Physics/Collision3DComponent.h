#pragma once

#include <Core/GamePlay/Component.h>
#include <Math/MyMath.h>

class btCollisionShape;
class btRigidBody;
class btDefaultMotionState;
namespace GuGu {
	enum class CollisionShape {
		//shape
		Box = 0,
		Sphere = 1,
		Capsule = 2
	};

	class Collision3DComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		Collision3DComponent();

		virtual ~Collision3DComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		CollisionShape getShape() const;
		void setShape(CollisionShape inCollisionShape);

		void recreateBulletShape();

		void setBoxHalfExtents(math::float3 inBoxHalfExtents);
		math::float3& getBoxHalfExtents();

		void setSphereRadius(float sphereRadius);
		float getSphereRadius() const;

		void setCapsuleRadius(float capsuleRadius);
		float getCapsuleRadius() const;

		void setCapsuleHeight(float inHeight);
		float getCapsuleHeight() const;

		void setMass(float newMass);
		float getMass() const;

		void updateMassAndInertia(float newMass);

		void syncFromPhysics();

		void syncToPhysics();

		virtual void PostLoad() override;

		void removeRigidBodyFromPhysics();

		void addRigidBodyToPhysics();

		bool isRigidBodyInPhysics();

		void setKinematic(bool isKinematic);
		bool getKinematic() const;

		//shape
		CollisionShape m_shape;

		math::float3 m_boxHalfExtents;
		float m_sphereRadius; //sphere
		float m_capsuleRadius; //capsule
		float m_capsuleHeight; //capsule

		//质量(质量为0是static，>0是dynamic)
		float m_mass;

		//质量为0，这个为 true，则是 kinematic (用于角色控制器等)
		bool m_bKinematic;

		//摩擦力
		float m_friction;

		//弹性系数
		float m_restitution;

		std::shared_ptr<btCollisionShape> m_collisionShape;
		std::shared_ptr<btRigidBody> m_rigidBody;
		std::shared_ptr<btDefaultMotionState> m_motionState;
	};
}