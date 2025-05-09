#include <pch.h>

#include "ReflectionDatabase.h"

#include "Type.h"
#include "TypeInfo.h"

//todo:这里修复
#define REGISTER_NATIVE_TYPE(type)		\
	{									\
		auto id = AllocateType(#type);	\
		auto& handle = types[id];		\
		TypeInfo<type>::Register(id, handle, true); \
	}											    \
//todo:这里修复
#define REGISTER_NATIVE_TYPE_VARIANTS(type) \
	REGISTER_NATIVE_TYPE(type)				\
	REGISTER_NATIVE_TYPE(type*)				\
	REGISTER_NATIVE_TYPE(const type*)		\

#define REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(type) \
	REGISTER_NATIVE_TYPE_VARIANTS(type)				\
	types[typeidof(type)].SetArrayConstructor<type>(); \

namespace GuGu {
	namespace meta {
		ReflectionDatabase::ReflectionDatabase(void)
			: types(1)
			, m_nextID(1)
		{
			types[InvalidTypeID].name = "UNKNOWN";

			//注册所有native类型变体，在任何事物之前
			REGISTER_NATIVE_TYPE_VARIANTS(void);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(int);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(unsigned int);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(bool);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(float);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(double);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(GuGuUtf8Str);
			REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(uint8_t);

			auto& stringType = types[typeidof(GuGuUtf8Str)];

			//:显式地为字符串添加构造函数
			stringType.AddConstructor<GuGuUtf8Str, false, false>();
			stringType.AddConstructor<GuGuUtf8Str, false, true>();
		}
		ReflectionDatabase::~ReflectionDatabase()
		{
		}
		ReflectionDatabase& ReflectionDatabase::Instance(void)
		{
			static ReflectionDatabase instance;

			return instance;
		}
		TypeID ReflectionDatabase::AllocateType(const GuGuUtf8Str& name)
		{
			auto search = ids.find(name);

			//已经定义了
			if (search != ids.end())
				return InvalidTypeID;

			types.emplace_back(name);//types的下标索引对应m_nextID

			auto id = m_nextID++;

			ids[name] = id;//name to id

			return id;
		}
	}
}

