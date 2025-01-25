#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Type.h>

#include "Component.h"

namespace GuGu {
	class GameObject : public meta::Object {

	public:
		GameObject();

		virtual ~GameObject();

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		virtual void OnSerialize(nlohmann::json& output) const override;

		virtual void OnDeserialize(const nlohmann::json& input) override;

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

	protected:
		Array<std::shared_ptr<Component>> m_components;

		Array<std::shared_ptr<GameObject>> m_childrens;

		GuGuUtf8Str m_name;
	};
}