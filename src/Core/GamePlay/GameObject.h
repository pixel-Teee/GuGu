#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>

#include "Component.h"

namespace GuGu {
	class GameObject : public meta::Object, public std::enable_shared_from_this<GameObject> {

	public:
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
			}
			return nullptr;
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

		void addComponent(std::shared_ptr<Component> inComponent);

		void setComponents(const Array<std::shared_ptr<Component>>& components);

		void deleteComponent(const GuGuUtf8Str& componentTypeName);

		Array<std::shared_ptr<Component>> getComponents();

		void setChildrens(const Array<std::shared_ptr<GameObject>>& childrens);

		Array<std::shared_ptr<GameObject>> getChildrens();

		std::weak_ptr<GameObject>& getParentGameObject();

		const std::weak_ptr<GameObject>& getParentGameObject() const;

		void setParentGameObject(const std::weak_ptr<GameObject>& inGameObject);

		const GuGuUtf8Str& getGameObjectName() const;

		uint32_t getId() const;
	protected:
		Array<std::shared_ptr<Component>> m_components;

		Array<std::shared_ptr<GameObject>> m_childrens;

		std::weak_ptr<GameObject> m_parentGameObject;

		GuGuUtf8Str m_name;

		uint32_t m_id;//don't save
	};
}