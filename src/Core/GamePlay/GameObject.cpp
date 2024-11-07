#include <pch.h>

#include "GameObject.h"

namespace GuGu {
	GameObject::GameObject()
	{
	}
	GameObject::~GameObject()
	{
	}
	void GameObject::Update(float fElapsedTimeSeconds)
	{
		for (int32_t i = 0; i < m_components.size(); ++i)
		{
			m_components[i]->Update(fElapsedTimeSeconds);
		}
	}
	meta::Type GameObject::GetType() const
	{
		return meta::Type();
	}
	meta::Object* GameObject::Clone(void) const
	{
		return nullptr;
	}
	void GameObject::OnSerialize(nlohmann::json& output) const
	{
	}
	void GameObject::OnDeserialize(const nlohmann::json& input)
	{
	}
}
