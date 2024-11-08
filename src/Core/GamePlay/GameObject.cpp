#include <pch.h>

#include "GameObject.h"
#include "TransformComponent.h"

namespace GuGu {
	GameObject::GameObject()
	{
	}
	GameObject::~GameObject()
	{
	}
	void GameObject::Update(float fElapsedTimeSeconds)
	{
		//1.更新变化组件
		getComponent<TransformComponent>()->Update(fElapsedTimeSeconds);
		//2.更新材质组件
		
		//for (int32_t i = 0; i < m_components.size(); ++i)
		//{
		//	m_components[i]->Update(fElapsedTimeSeconds);
		//}

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
