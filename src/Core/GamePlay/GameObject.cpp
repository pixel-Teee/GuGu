#include <pch.h>

#include "GameObject.h"
#include "TransformComponent.h"
#include "LightComponent.h"

#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/Reflection/MetaManager.h>

namespace GuGu {
	GameObject::GameObject()
	{
		const meta::DisplayName* displayName = typeof(GameObject).GetMeta().GetProperty<meta::DisplayName>();
		if (displayName)
		{
			m_name = displayName->m_displayName;
		}
		//m_name = typeof(GameObject).GetMeta().GetProperty<meta::DisplayName>()->m_displayName;

		static uint32_t id = 0;
		m_id = ++id;
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
		if (getComponent<LightComponent>())
		{
			getComponent<LightComponent>()->Update(fElapsedTimeSeconds);
		}
		
	}
	meta::Type GameObject::GetType() const
	{
		return typeof(GameObject);
	}
	meta::Object* GameObject::Clone(void) const
	{
		GameObject* gameObject = new GameObject();
		gameObject->m_name = m_name;
		gameObject->m_components = m_components;
		gameObject->m_childrens = m_childrens;
		return gameObject;
	}
	void GameObject::OnSerialize(nlohmann::json& output) const
	{
	}
	void GameObject::OnDeserialize(const nlohmann::json& input)
	{
	}

	void GameObject::PostLoad()
	{

	}

	void GameObject::addComponent(std::shared_ptr<Component> inComponent)
	{
		inComponent->setParentGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
		m_components.push_back(inComponent);
	}
	void GameObject::setComponents(const Array<std::shared_ptr<Component>>& components)
	{
		m_components = components;
	}

	void GameObject::deleteComponent(const GuGuUtf8Str& componentTypeName)
	{
		//could not delete transform component
		if (componentTypeName == typeof(TransformComponent).GetName())
			return;
		int32_t foundPos = -1;
		for (int32_t i = 0; i < m_components.size(); ++i)
		{
			if (m_components[i]->GetType() == meta::Type::GetFromName(componentTypeName))
			{
				foundPos = i;
				break;
			}
		}
		if (foundPos != -1)
		{
			//delete
			m_components.erase(m_components.begin() + foundPos);
		}
	}

	Array<std::shared_ptr<Component>> GameObject::getComponents()
	{
		return m_components;
	}
	void GameObject::setChildrens(const Array<std::shared_ptr<GameObject>>& childrens)
	{
		m_childrens = childrens;
		for (const auto& children : m_childrens)
		{
			children->setParentGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
		}
	}

	Array<std::shared_ptr<GameObject>> GameObject::getChildrens()
	{
		return m_childrens;
	}

	const std::weak_ptr<GameObject>& GameObject::getParentGameObject() const
	{
		return m_parentGameObject;
	}

	std::weak_ptr<GameObject>& GameObject::getParentGameObject()
	{
		return m_parentGameObject;
	}

	void GameObject::setParentGameObject(const std::weak_ptr<GameObject>& inGameObject)
	{
		m_parentGameObject = inGameObject;
	}

	const GuGuUtf8Str& GameObject::getGameObjectName() const
	{
		//return typeof(GameObject).GetName();
		return m_name;
	}

	uint32_t GameObject::getId() const
	{
		return m_id;//只在运行期间有效
	}

}
