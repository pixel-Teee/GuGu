#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>

#include "Component.h"

namespace GuGu {
	class Level;
	class World;
	class GameObject : public meta::Object {

	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GameObject();

		virtual ~GameObject();

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		virtual void OnSerialize(nlohmann::json& output) const override;

		virtual void OnDeserialize(const nlohmann::json& input) override;

		virtual void PostLoad() override;

		template<typename T>
		std::shared_ptr<T> getComponent() 
		{
			for (auto& component : m_components)
			{ 
				if (component->GetType() == typeof(T)) //需要注册指针类型
				{
					return std::static_pointer_cast<T>(component);
				}
				//check parent class
				std::set<meta::Type> parentTypes = component->GetType().GetBaseClasses();
				if (parentTypes.find(typeof(T)) != parentTypes.end())
				{
					return std::static_pointer_cast<T>(component);
				}
			}
			return nullptr;
		}

		template<typename ParentType>
		std::vector<std::shared_ptr<Component>> getComponentParentClassIs()
		{
			std::vector<std::shared_ptr<Component>> res;
			for (auto& component : m_components)
			{
				std::set<meta::Type> baseClasses = component->GetType().GetBaseClasses();
				if (baseClasses.find(typeof(ParentType)) != baseClasses.end()) //需要注册指针类型
				{
					res.push_back(component);
					return res;
				}
			}
			return std::vector<std::shared_ptr<Component>>();
		}

		template<typename T>
		std::shared_ptr<T> getComponent() const
		{
			for (auto& component : m_components)
			{
				if (component->GetType() == typeof(T)) //需要注册指针类型
				{
					return std::static_pointer_cast<T>(component);
				}
			}
			return nullptr;
		}

		std::shared_ptr<Component> addComponent(std::shared_ptr<Component> inComponent);

		std::shared_ptr<Component> addComponentFromName(const GuGuUtf8Str& componentTypeName);

		void setComponents(const Array<std::shared_ptr<Component>>& components);

		void deleteComponent(const GuGuUtf8Str& componentTypeName);

		std::shared_ptr<Component> getComponent(const GuGuUtf8Str& componentTypeName);

		Array<std::shared_ptr<Component>> getComponents();

		void setChildrens(const Array<std::shared_ptr<GameObject>>& childrens);

		Array<std::shared_ptr<GameObject>>& getChildrens();

		std::shared_ptr<GameObject> getChildren(const GuGuUtf8Str& gameObjectName);

		std::weak_ptr<GameObject>& getParentGameObject();

		const std::weak_ptr<GameObject>& getParentGameObject() const;

		void setParentGameObject(const std::weak_ptr<GameObject>& inGameObject);

		const GuGuUtf8Str& getGameObjectName() const;

		uint32_t getId() const;

		void addChildren(std::shared_ptr<GameObject> children);

		void deleteChildren(std::shared_ptr<GameObject> children);

		void insertChildren(std::shared_ptr<GameObject> children, int32_t index);

		int32_t findIndex(std::shared_ptr<GameObject> children);

		void setName(const GuGuUtf8Str& inName);

		GuGuUtf8Str& getName();

		//game object 所在的 level
		std::shared_ptr<Level> getCurrentLevel();

		std::shared_ptr<World> getWorld();

		void clearChildrens();

		void traverseGameObjectTrees(std::function<void(std::shared_ptr<GameObject> inGameObject)> callBack);

		void setPrefab(const GuGuUtf8Str& prefabAssetGuid);
		GuGuUtf8Str getPrefab() const;
		//这个 game object 是否属于预制体的一部分
		bool isPrefabChildren() const;
		//获取预制体根对象
		std::shared_ptr<GameObject> getPrefabRootGameObject();
	protected:
		Array<std::shared_ptr<Component>> m_components;

		Array<std::shared_ptr<GameObject>> m_childrens;

		std::weak_ptr<GameObject> m_parentGameObject;

		GuGuUtf8Str m_name;

		uint32_t m_id;//don't save

		//file path to prefab
		//如果这个 game object 关联了一个预制体，这个变量将不为空
		GuGuUtf8Str m_prefabAssetGuid;
	};
}