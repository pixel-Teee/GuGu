#include <pch.h>

#include "Object.h"
#include <Core/Reflection/MetaProperty/DisplayName.h>
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuMetaObject()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::meta::Object");
		auto& type = db.types[id];
		meta::TypeInfo<meta::Object>::Register(id, type, true, "E6023C42-0DA9-4F76-8290-CDB926C881E3");

		type.meta = {
			std::make_pair(typeof(meta::DisplayName), meta::MetaPropertyInitializer<meta::DisplayName>("Object"))
		};
		return true;
	}

	namespace meta {
		IMPLEMENT_INITIAL_BEGIN(Object)
			ADD_PRIORITY(meta::DisplayName)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuMetaObject)
		IMPLEMENT_INITIAL_END
	}
}