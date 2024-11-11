#include <pch.h>

#include "TransformComponent.h"
#include "GameObject.h"

namespace GuGu {
	TransformComponent::TransformComponent()
	{
	}
	TransformComponent::~TransformComponent()
	{
	}
	void TransformComponent::Update(float fElapsedTimeSeconds)
	{	
		UpdateLocalModelMatrix();
		UpdateGlobalTransform();
	}
	void TransformComponent::UpdateLocalModelMatrix()
	{
		//update local transform
		math::daffine3 transform = math::scaling(m_Scaling);
		transform *= m_Rotation.toAffine();
		transform *= math::translation(m_Translation);
		m_LocalTransform = transform;
	}
	void TransformComponent::UpdateGlobalTransform()
	{
		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner != nullptr)
		{
			std::shared_ptr<TransformComponent> parentTransformComponent =
				owner->getComponent<TransformComponent>();
			m_GlobalTransform = m_LocalTransform * parentTransformComponent->GetLocalToWorldTransform();
		}
		else
		{
			m_GlobalTransform = m_LocalTransform;
		}
		m_GlobalTransformFloat = math::affine3(m_GlobalTransform);
	}
	void TransformComponent::SetTransform(const math::double3* translation, const math::dquat* rotation, const math::double3* scaling)
	{
		if (scaling) m_Scaling = *scaling;
		if (rotation) m_Rotation = *rotation;
		if (translation) m_Translation = *translation;
	}
	void TransformComponent::SetScaling(const math::double3& scaling)
	{
		SetTransform(nullptr, nullptr, &scaling);
	}
	void TransformComponent::SetRotation(const math::dquat& rotation)
	{
		SetTransform(nullptr, &rotation, nullptr);
	}
	void TransformComponent::SetTranslation(const math::double3& translation)
	{
		SetTransform(&translation, nullptr, nullptr);
	}
}