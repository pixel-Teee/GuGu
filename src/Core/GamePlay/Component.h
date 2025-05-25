#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {
	class GameObject;
	class Component : public meta::Object{
	public:
		DECLARE_INITIAL

		DECLARE_INITIAL_FIELDS
		virtual meta::Type GetType() const override;

		virtual ~Component() {}

		virtual void PostLoad() {}

		virtual void Update(float fElapsedTimeSeconds) {}

		virtual Object* Clone(void) const override { return nullptr; }

		virtual void OnSerialize(nlohmann::json& output) const {}
		virtual void OnDeserialize(const nlohmann::json& input) {}

		const std::weak_ptr<GameObject>& getParentGameObject() const;

		std::weak_ptr<GameObject>& getParentGameObject();

		void setParentGameObject(const std::weak_ptr<GameObject>& inGameObject);

		std::weak_ptr<GameObject> m_owner;//parent
	};
}