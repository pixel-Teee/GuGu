#include <pch.h>

#include "LuaTable.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuLuaTable()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::LuaTable");
		auto& type = db.types[id];
		meta::TypeInfo<LuaTable>::Register(id, type, true, "AD574E00-12DC-424E-8C0A-694A0697A12F");

		auto typeID = typeidof(LuaTable);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<LuaTable>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<LuaTable>();

			type.AddConstructor<LuaTable, false, false>({});

			type.AddConstructor<LuaTable, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<LuaTable>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::LuaTable>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<LuaTable>>::Register(id, type, false, "FE18B5C7-834A-4FA8-927E-116BE5B7943A");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::LuaTable>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<LuaTable>>::Register(id, type, false, "56162C6A-CBDC-4F83-86CB-CB0053B4AC45");
		}
		return true;
	}

	static bool registerGuGuLuaTableFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(LuaTable).GetID()];

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(LuaTable)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuLuaTable)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(LuaTable)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuLuaTableFields)
	IMPLEMENT_INITIAL_FIELDS_END


	meta::Type LuaTable::GetType() const
	{
		return typeof(LuaTable);
	}

}