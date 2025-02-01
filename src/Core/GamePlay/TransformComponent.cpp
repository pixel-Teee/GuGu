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

	meta::Object* TransformComponent::Clone(void) const
	{
		TransformComponent* transformComponent = new TransformComponent();
		transformComponent->m_Translation = m_Translation;
		transformComponent->m_Scaling = m_Scaling;
		transformComponent->m_Rotation = m_Rotation;
		transformComponent->m_owner = m_owner;//todo:这里要修复
		return transformComponent;
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
			std::shared_ptr<GameObject> parentGameObject = owner->getParentGameObject().lock();

			if (parentGameObject != nullptr)
			{
				std::shared_ptr<TransformComponent> parentTransformComponent =
					parentGameObject->getComponent<TransformComponent>();
				m_GlobalTransform = m_LocalTransform * parentTransformComponent->GetLocalToWorldTransform();
			}
			else
			{
				m_GlobalTransform = m_LocalTransform;
			}
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

	math::double3 TransformComponent::getTranslation() const
	{
		return m_Translation;
	}

	math::dquat TransformComponent::getRotation() const
	{
		return m_Rotation;
	}

	math::double3 TransformComponent::getScaling() const
	{
		return m_Scaling;
	}

	meta::Type TransformComponent::GetType() const
	{
		return typeof(TransformComponent);
	}
}