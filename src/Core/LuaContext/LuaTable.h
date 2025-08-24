#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {
	class LuaTable : public meta::Object
	{
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		virtual meta::Type GetType() const override;

		virtual ~LuaTable() {}

		virtual void PostLoad() {}

		virtual void Update(float fElapsedTimeSeconds) {}

		virtual Object* Clone(void) const override { return nullptr; }

		virtual void OnSerialize(nlohmann::json& output) const {}
		virtual void OnDeserialize(const nlohmann::json& input) {}

		int32_t refId = -1;
	};
}