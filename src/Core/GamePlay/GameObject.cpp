#include <pch.h>

#include "GameObject.h"

//------component------
#include "TransformComponent.h"
#include "LightComponent.h"
#include <Core/GamePlay/ScriptComponent.h>
#include <Core/GamePlay/GameUI/CanvasComponent.h>
#include <Core/GamePlay/GameUI/UITransformComponent.h>
#include <Core/GamePlay/TransformComponent.h>
//------component------
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>

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

		type.AddConstructor<GuGu::GameObject, false, false>({});

		type.AddConstructor<GuGu::GameObject, true, true>({});

		//dependent meta::Object
		type.LoadBaseClasses(db, id, { typeof(meta::Object) });

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

		return true;
	}
	static bool registerGuGuGameObjectFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(GuGu::GameObject).GetID()];

		type.meta = {
			std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("GameObject"))
		};

		type.AddField<GameObject, GuGuUtf8Str>("m_name",
			(meta::FieldGetter<GameObject, GuGuUtf8Str&, true>::Signature) & GameObject::getName,
			(meta::FieldSetter<GameObject, GuGuUtf8Str, true>::Signature) & GameObject::setName, {});

		type.AddField<GameObject, Array<std::shared_ptr<Component>>>("m_components",
			(meta::FieldGetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::getComponents,
			(meta::FieldSetter<GameObject, Array<std::shared_ptr<Component>>, true>::Signature) & GameObject::setComponents, {});

		type.AddField<GameObject, Array<std::shared_ptr<GameObject>>>("m_childrens",
			(meta::FieldGetter<GameObject, Array<std::shared_ptr<GameObject>>&, true>::Signature) & GameObject::getChildrens,
			(meta::FieldSetter<GameObject, Array<std::shared_ptr<GameObject>>&, true>::Signature) & GameObject::setChildrens, {});

		type.AddField<GameObject, std::weak_ptr<GameObject>>("m_parentGameObject",
			(meta::FieldGetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::getParentGameObject,
			(meta::FieldSetter<GameObject, std::weak_ptr<GameObject>&, true>::Signature) & GameObject::setParentGameObject, {});

		//method functions
		type.AddMethod("setName", &GameObject::setName, {});

		std::shared_ptr<Component>(GameObject::*getComponentPtr)(const GuGuUtf8Str& componentTypeName) = &GameObject::getComponent; //non const
		type.AddMethod("getComponent", getComponentPtr, {});

		std::shared_ptr<GameObject>(GameObject::* getChildrenPtr)(const GuGuUtf8Str& gameObjectName) = &GameObject::getChildren; //non const
		type.AddMethod("getChildren", getChildrenPtr, {});

		type.AddMethod("getCurrentLevel", &GameObject::getCurrentLevel, {});

		type.AddMethod("getWorld", &GameObject::getWorld, {});

		type.AddMethod("addComponent", &GameObject::addComponent, {});

		type.AddMethod("addChildren", &GameObject::addChildren, {});

		type.AddMethod("addComponentFromName", &GameObject::addComponentFromName, {});

		type.AddMethod("clearChildrens", &GameObject::clearChildrens, {});

		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(GameObject)
		ADD_PRIORITY(meta::Object)
		ADD_PRIORITY(Component)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGameObject)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(GameObject)
		ADD_PRIORITY_FIELDS(meta::DisplayName)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuGameObjectFields)
	IMPLEMENT_INITIAL_FIELDS_END
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
		if (getComponent<ScriptComponent>())
		{
			getComponent<ScriptComponent>()->Update(fElapsedTimeSeconds);
		}

		if (getComponent<CanvasComponent>())
		{
			getComponent<CanvasComponent>()->Update(fElapsedTimeSeconds);
		}
		
		//1.更新变化组件
		getComponent<TransformComponent>()->Update(fElapsedTimeSeconds);

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
		for (int32_t i = 0; i < m_components.size(); ++i)
		{
			Component* component = static_cast<Component*>(m_components[i]->Clone());
			//cast to shared ptr
			std::shared_ptr<Component> componentPtr(component);
			gameObject->addComponent(componentPtr);
		}
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

	std::shared_ptr<Component> GameObject::addComponent(std::shared_ptr<Component> inComponent)
	{
		if (inComponent->GetType() == typeof(UITransformComponent))
		{
			if (getComponent<UITransformComponent>())
			{
				return getComponent<UITransformComponent>();
			}
			if (getComponent<TransformComponent>())
			{
				deleteComponent("GuGu::TransformComponent");
			}
		}
		if (inComponent->GetType() == typeof(TransformComponent))
		{
			if (getComponent<TransformComponent>())
			{
				return getComponent<TransformComponent>();
			}
			if (getComponent<UITransformComponent>())
			{
				deleteComponent("GuGu::UITransformComponent");
			}
		}
		inComponent->setParentGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
		m_components.push_back(inComponent);

		return inComponent;
	}

	std::shared_ptr<Component> GameObject::addComponentFromName(const GuGuUtf8Str& componentTypeName)
	{
		const meta::Constructor& constructor = meta::Type::GetFromName(componentTypeName).GetDynamicConstructor();
		const meta::Variant& componentInstance = constructor.Invoke();
		Component* instancePointer = static_cast<Component*>(componentInstance.getBase()->GetPtr());
		return addComponent(std::shared_ptr<Component>(instancePointer));
	}

	void GameObject::setComponents(const Array<std::shared_ptr<Component>>& components)
	{
		m_components = components;
	}

	void GameObject::deleteComponent(const GuGuUtf8Str& componentTypeName)
	{
		//could not delete transform component
// 		if (componentTypeName == typeof(TransformComponent).GetName())
// 			return;
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

	std::shared_ptr<Component> GameObject::getComponent(const GuGuUtf8Str& componentTypeName)
	{
		meta::Type componentType = meta::Type::GetFromName(componentTypeName);
		for (auto& component : m_components)
		{
			if (component->GetType() == componentType) //需要注册指针类型
			{
				return component;
			}
		}
		return nullptr;
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
			std::shared_ptr<GameObject> parent = std::static_pointer_cast<GameObject>(shared_from_this());
			children->setParentGameObject(parent);
		}
	}

	Array<std::shared_ptr<GameObject>>& GameObject::getChildrens()
	{
		return m_childrens;
	}

	std::shared_ptr<GameObject> GameObject::getChildren(const GuGuUtf8Str& gameObjectName)
	{
		for (const auto& children : m_childrens)
		{
			if(children->getName() == gameObjectName)
				return children;
		}
		return nullptr;
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

	void GameObject::addChildren(std::shared_ptr<GameObject> children)
	{
		children->setParentGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
		m_childrens.push_back(children);

		//attach
		//if (!World::getWorld()->m_onObjectAttached.empty())
		//{
		//	for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
		//	{
		//		World::getWorld()->m_onObjectAttached[i](children, std::static_pointer_cast<GameObject>(shared_from_this()));
		//	}
		//}
	}

	void GameObject::insertChildren(std::shared_ptr<GameObject> children, int32_t index)
	{
		m_childrens.insert(m_childrens.begin() + index, children);
		children->setParentGameObject(std::static_pointer_cast<GameObject>(shared_from_this()));
	}

	int32_t GameObject::findIndex(std::shared_ptr<GameObject> children)
	{
		for (int32_t i = 0; i < m_childrens.size(); ++i)
		{
			if (m_childrens[i] == children)
				return i;
		}
		return -1;
	}

	void GameObject::setName(const GuGuUtf8Str& inName)
	{
		m_name = inName;
	}

	GuGuUtf8Str& GameObject::getName()
	{
		return m_name;
	}

	std::shared_ptr<Level> GameObject::getCurrentLevel()
	{
		return World::getWorld()->getCurrentLevel();
	}

	std::shared_ptr<World> GameObject::getWorld()
	{
		return World::getWorld();
	}

	void GameObject::clearChildrens()
	{
		for (int32_t i = 0; i < m_childrens.size(); ++i)
		{
			m_childrens[i]->setParentGameObject(std::shared_ptr<GameObject>());//clear
		}
		m_childrens.clear();
	}

	void GameObject::traverseGameObjectTrees(std::function<void(std::shared_ptr<GameObject> inGameObject)> callBack)
	{
		if (callBack)
		{
			callBack(std::static_pointer_cast<GameObject>(shared_from_this()));
		}
		
		for (int32_t i = 0; i < m_childrens.size(); ++i)
		{
			traverseGameObjectTrees(callBack);
		}
	}

}
