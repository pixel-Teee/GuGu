#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {
	class GameObject;
	class Prefab : public meta::Object {
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		virtual meta::Type GetType() const override;

		Prefab();

		virtual ~Prefab();

		virtual void PostLoad();
		virtual void Update(float fElapsedTimeSeconds);
		virtual Object* Clone(void) const override;
		virtual void OnSerialize(nlohmann::json& output) const;
		virtual void OnDeserialize(const nlohmann::json& input);
	private:
		Array<std::shared_ptr<GameObject>> m_objects; //game object tree
	};
}