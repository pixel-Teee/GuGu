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
// 
// 				type.AddField<ComplexType, int>("a", (meta::FieldGetter<ComplexType, int, true>::Signature)&ComplexType::gettera, 
// 					(meta::FieldSetter<ComplexType, int, true>::Signature)
// 					nullptr);
// 
// 				type.AddField<ComplexType, double>("b", (meta::FieldGetter<ComplexType, double, true>::Signature) & ComplexType::getterb,
// 					(meta::FieldSetter<ComplexType, double, true>::Signature)
// 					nullptr);
// 
// 				type.AddField<ComplexType, float>("c", (meta::FieldGetter<ComplexType, float, true>::Signature) & ComplexType::getterc,
// 					(meta::FieldSetter<ComplexType, float, true>::Signature)
// 					nullptr);
// 
// 				type.AddField<ComplexType, GuGuUtf8Str>("str", (meta::FieldGetter<ComplexType, GuGuUtf8Str, true>::Signature)&ComplexType::getterstr,
// 					(meta::FieldSetter<ComplexType, GuGuUtf8Str, true>::Signature)
// 					nullptr);
// 
// 				type.AddField<ComplexType, Array<int>>("counts", (meta::FieldGetter<ComplexType, Array<int>&, true>::SignatureConst)&ComplexType::gettercounts,
// 					(meta::FieldSetter<ComplexType, Array<int>, true>::Signature)
// 					nullptr);


				//直接获取字段的注册方法
				type.AddField<ComplexType, int>("a", (meta::FieldGetter<ComplexType, int, false>::Signature) & ComplexType::a,
					(meta::FieldSetter<ComplexType, int, true>::Signature)
					nullptr);

				type.AddField<ComplexType, double>("b", (meta::FieldGetter<ComplexType, double, false>::Signature) & ComplexType::b,
					(meta::FieldSetter<ComplexType, double, true>::Signature)
					nullptr);

				type.AddField<ComplexType, float>("c", (meta::FieldGetter<ComplexType, float, false>::Signature) & ComplexType::c,
					(meta::FieldSetter<ComplexType, float, true>::Signature)
					nullptr);

				type.AddField<ComplexType, GuGuUtf8Str>("str", (meta::FieldGetter<ComplexType, GuGuUtf8Str, false>::Signature) & ComplexType::str,
					(meta::FieldSetter<ComplexType, GuGuUtf8Str, true>::Signature)
					nullptr);

				type.AddField<ComplexType, Array<int>>("counts", (meta::FieldGetter<ComplexType, Array<int>, false>::Signature) & ComplexType::counts,
					(meta::FieldSetter<ComplexType, Array<int>, true>::Signature)
					nullptr);
				
				meta::TypeInfo<ComplexType>::Defined = true;
			}
		}

		ComplexType test;
		test.a = 3;
		test.b = 4;
		test.c = 5;
		test.str = "ddd";
		test.counts.push_back(5);
		test.counts.push_back(4);
		test.counts.push_back(3);
		test.counts.push_back(2);

		GuGuUtf8Str res = meta::Variant(test).SerializeJson().dump();
		GuGu_LOGD("%s", res.getStr());
	}
}