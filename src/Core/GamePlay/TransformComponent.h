#pragma once

#include "Component.h"

#include <Core/Math/MyMath.h>

namespace GuGu {
	class GameObject;
	//transform component
	class TransformComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		TransformComponent();

		virtual ~TransformComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		void UpdateLocalModelMatrix();

		void UpdateGlobalTransform();

		const math::daffine3& GetLocalToWorldTransform() const { return m_GlobalTransform; }
		const math::affine3& GetLocalToWorldTransformFloat() const { return m_GlobalTransformFloat; }

		const math::daffine3& GetLocalTransform() const { return m_LocalTransform; }
		const math::affine3& GetLocalTransformFloat() const { return math::affine3(m_LocalTransform); }

		void SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling);
		void SetScaling(math::double3 scaling);
		void SetRotationQuat(math::dquat rotation);
		void SetRotator(math::Rotator inRotator);
		void SetTranslation(math::double3 translation);

		math::double3 getTranslation() const;
		math::double3& getTranslation();
		math::dquat getRotationQuat() const;
		math::Rotator& getRotator();
		math::Rotator getRotator() const;
		math::double3 getScaling() const;
		math::double3& getScaling();

		//get parent translation
		//math::double3 getAccumulatedTranslation() const;

		virtual meta::Type GetType() const override;
	private:
		//math::dquat m_Rotation = math::dquat::identity();
		//rotate-z, rotate-y, rotate-x order
		math::Rotator m_Rotation = math::double3(0, 0, 0);//roll, pitch, yaw
		math::double3 m_Scaling = 1.0;
		math::double3 m_Translation = math::double3(0, 0, 10);
		math::daffine3 m_LocalTransform = math::daffine3::identity();
		math::daffine3 m_GlobalTransform = math::daffine3::identity();
		math::affine3 m_GlobalTransformFloat = math::affine3::identity();
	};
}