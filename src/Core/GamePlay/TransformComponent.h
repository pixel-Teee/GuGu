#pragma once

#include "Component.h"

#include <Core/Math/MyMath.h>

namespace GuGu {
	class GameObject;
	//transform component
	class TransformComponent : public Component
	{
	public:
		TransformComponent();

		virtual ~TransformComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		void UpdateLocalModelMatrix();

		void UpdateGlobalTransform();

		const math::daffine3& GetLocalToWorldTransform() const { return m_GlobalTransform; }
		const math::affine3& GetLocalToWorldTransformFloat() const { return m_GlobalTransformFloat; }

		void SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling);
		void SetScaling(const math::double3& scaling);
		void SetRotation(const math::dquat& rotation);
		void SetTranslation(const math::double3& translation);

		math::double3 getTranslation() const;
		math::dquat getRotation() const;
		math::double3 getScaling() const;

		virtual meta::Type GetType() const override;
	private:
		math::dquat m_Rotation = math::dquat::identity();
		math::double3 m_Scaling = 1.0;
		math::double3 m_Translation = math::double3(0, 0, 10);
		math::daffine3 m_LocalTransform = math::daffine3::identity();
		math::daffine3 m_GlobalTransform = math::daffine3::identity();
		math::affine3 m_GlobalTransformFloat = math::affine3::identity();
	};
}