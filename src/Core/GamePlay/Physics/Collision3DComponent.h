#pragma once

#include <Core/GamePlay/Component.h>

class btCollisionShape;
class btRigidBody;
namespace GuGu {
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

	private:
		//shape

		//质量
		float m_mass;

		//摩擦力
		float m_friction;

		//弹性系数
		float m_restitution;

		std::shared_ptr<btCollisionShape> m_collisionShape;
		std::shared_ptr<btRigidBody> m_rigidBody;
	};
}