#include <pch.h>

#include "GameObject.h"
#include "TransformComponent.h"
#include "LightComponent.h"

#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/Reflection/MetaManager.h>
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuGameObject()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GameObject");
		auto& type = db.types[id];
		meta::TypeInfo<GuGu::GameObject>::Register(id, type, true, "764220D9-31E0-448B-9612-79A47B570367");

		type.meta = {
			std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("GameObject"))
		};

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GameObject>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GameObject>>::Register(id, type, false, "5126D2F9-FCF4-4020-8BBB-187389965FDE");

			type.SetArrayConstructor<std::shared_ptr<GameObject>>();
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GameObject>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GameObject>>::Register(id, type, false, "E3D8C076-BFD3-4FD1-B108-5FBB37169DE3");
		}

		auto typeID = typeidof(GameObject);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GameObject>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GameObject>();

			type.AddConstructor<GameObject, false, false>({});

			type.AddConstructor<GameObject, true, true>({});

			type.AddField<GameObject, Array<std::shared_ptr<Component>>>("m_components",
				(meta::FieldGetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::getComponents,
				(meta::FieldSetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::setComponents, {});

			type.AddField<GameObject, Array<std::shared_ptr<GameObject>>>("m_childrens",
				(meta::FieldGetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::getChildrens,
				(meta::FieldSetter<GameObject, Array<std::shared_ptr<GameObject>>, true>::Signature) & GameObject::setChildrens, {});

			type.AddField<GameObject, std::weak_ptr<GameObject>>("m_parentGameObject",
				(meta::FieldGetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::getParentGameObject,
				(meta::FieldSetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::setParentGameObject, {});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<GameObject>::Defined = true;
		}
		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GameObject)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGameObject)
	IMPLEMENT_INITIAL_END
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
