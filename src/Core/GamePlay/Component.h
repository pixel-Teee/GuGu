#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {
	class Component : public meta::Object{
	public:
		virtual meta::Type GetType() const override;

		virtual ~Component() {}

		virtual void Update(float fElapsedTimeSeconds) {}

		virtual Object* Clone(void) const override { return nullptr; }

		virtual void OnSerialize(nlohmann::json& output) const {}
		virtual void OnDeserialize(const nlohmann::json& input) {}
	};
}