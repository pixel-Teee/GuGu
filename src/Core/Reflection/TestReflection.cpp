#include <pch.h>

#include "TestReflection.h"
#include "ReflectionDatabase.h"
#include "TypeInfo.h"

namespace GuGu {
	void testReflection()
	{
		//创建反射数据库
		auto& db = meta::ReflectionDatabase::Instance();

		//注册ComplexType反射数据
		{
			auto id = db.AllocateType("ComplexType");
			auto& type = db.types[id];
			meta::TypeInfo<ComplexType>::Register(id, type, true);
		}

		{
			auto id = db.AllocateType("ComplexTypePtr");
			auto& type = db.types[id];
			meta::TypeInfo<ComplexType*>::Register(id, type, false);
		}

		{
			auto id = db.AllocateType("const ComplexTypePtr");
			auto& type = db.types[id];

			meta::TypeInfo<const ComplexType*>::Register(id, type, false);
		}

		{
			auto typeID = typeidof(ComplexType);
			if (typeID != meta::InvalidTypeID && !meta::TypeInfo<ComplexType>::Defined)
			{
				auto& type = db.types[typeID];

				type.AddField<ComplexType, int>("a", (meta::FieldGetter<ComplexType, int, true>::Signature)&ComplexType::gettera, 
					(meta::FieldSetter<ComplexType, int, true>::Signature)
					nullptr);

				type.AddField<ComplexType, double>("b", (meta::FieldGetter<ComplexType, double, true>::Signature) & ComplexType::getterb,
					(meta::FieldSetter<ComplexType, double, true>::Signature)
					nullptr);

				type.AddField<ComplexType, float>("c", (meta::FieldGetter<ComplexType, float, true>::Signature) & ComplexType::getterc,
					(meta::FieldSetter<ComplexType, float, true>::Signature)
					nullptr);

				type.AddField<ComplexType, GuGuUtf8Str>("str", (meta::FieldGetter<ComplexType, GuGuUtf8Str, true>::Signature)&ComplexType::getterstr,
					(meta::FieldSetter<ComplexType, GuGuUtf8Str, true>::Signature)
					nullptr);
				
				meta::TypeInfo<ComplexType>::Defined = true;
			}
		}

		ComplexType test;
		test.a = 3;
		test.b = 4;
		test.c = 5;
		test.str = "ddd";

		GuGuUtf8Str res = meta::Variant(test).SerializeJson().dump();
		GuGu_LOGD("%s", res.getStr());
	}
}