#include <pch.h>

#include "ReflectionDatabase.h"

#include "Type.h"
#include "TypeInfo.h"

//todo:这里修复
#define REGISTER_NATIVE_TYPE(type, guidStr)					 \
	{														 \
		auto id = AllocateType(#type);						 \
		auto& handle = types[id];							 \
		TypeInfo<type>::Register(id, handle, true, guidStr); \
	}														 \
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
			//REGISTER_NATIVE_TYPE_VARIANTS(void);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(int);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(unsigned int);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(bool);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(float);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(double);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(GuGuUtf8Str);
			//REGISTER_NATIVE_TYPE_VARIANTS_W_ARRAY(uint8_t);

			REGISTER_NATIVE_TYPE(void, "BE31C9A9-177F-44CF-8A34-A82867B5E286");
			REGISTER_NATIVE_TYPE(int, "5FC300E1-7B2E-40D4-8B0A-9B5BD648C6F2");
			REGISTER_NATIVE_TYPE(int*, "909E131C-E7D9-4678-AFE9-3D9241C22735");
			REGISTER_NATIVE_TYPE(const int*, "7AEA86B3-FB2F-4483-85ED-F68FD72E3D76");
			types[typeidof(int)].SetArrayConstructor<int>();

			REGISTER_NATIVE_TYPE(bool, "FA769DC5-B6EF-4DE6-A4CA-F122F2546EF1");
			REGISTER_NATIVE_TYPE(bool*, "30A0EC0D-FEA7-402F-BF02-A501F2C20CF2");
			REGISTER_NATIVE_TYPE(const bool*, "5CE6534D-5624-4B0C-BE02-67D1D058CFB0");
			types[typeidof(bool)].SetArrayConstructor<bool>();

			REGISTER_NATIVE_TYPE(unsigned int, "5679E77D-D9B9-462F-AFA1-AD81B582B8B7");
			REGISTER_NATIVE_TYPE(unsigned int*, "B0BCF504-E81A-4BD1-B131-1243DA006C22");
			REGISTER_NATIVE_TYPE(const unsigned int*, "25BD3A2F-BDA2-43E8-B305-7D4D97144279");
			types[typeidof(unsigned int)].SetArrayConstructor<unsigned int>();

			REGISTER_NATIVE_TYPE(float, "CFB617C7-03DB-4751-9C10-99227A07B1B1");
			REGISTER_NATIVE_TYPE(float*, "F2CAD8F3-CD11-4AC6-9EB3-F59846AA4708");
			REGISTER_NATIVE_TYPE(const float*, "A2D56149-6F75-4F03-AF1A-23D9FBCB934B");
			types[typeidof(float)].SetArrayConstructor<float>();

			REGISTER_NATIVE_TYPE(double, "CADCA18F-A40A-40D3-BB6D-421181023B8F");
			REGISTER_NATIVE_TYPE(double*, "3A2B1BBF-FD58-4536-8FD5-4C87C8762EC2");
			REGISTER_NATIVE_TYPE(const double*, "C8046CEB-0ADA-4783-A734-7CC32C188B87");
			types[typeidof(double)].SetArrayConstructor<double>();

			REGISTER_NATIVE_TYPE(GuGuUtf8Str, "A99F776B-25FF-40F2-BEAA-25C511148D65");
			REGISTER_NATIVE_TYPE(GuGuUtf8Str*, "71DE09A3-9AF1-433E-B092-65890FB1678A");
			REGISTER_NATIVE_TYPE(const GuGuUtf8Str*, "A3DDF409-BA86-4A90-9421-E6E3B677A994");
			//types[typeidof(GuGuUtf8Str)].SetArrayConstructor<GuGuUtf8Str>();

			REGISTER_NATIVE_TYPE(uint8_t, "745CBF56-CC95-4240-94FC-5BF7E2D9726A");
			REGISTER_NATIVE_TYPE(uint8_t*, "AFF7FB77-DB8D-4F96-A89E-AF41AAAD6300");
			REGISTER_NATIVE_TYPE(const uint8_t*, "BD14B25F-9A9C-49E6-AFF8-2C2D19A4CC64");
			types[typeidof(uint8_t)].SetArrayConstructor<uint8_t>();

			REGISTER_NATIVE_TYPE(uint16_t, "202C53E9-EF07-4BD8-9A28-0B3C3AD79074");
			REGISTER_NATIVE_TYPE(uint16_t*, "8F66A3BE-0445-474B-94B3-09D408F7BC62");
			REGISTER_NATIVE_TYPE(const uint16_t*, "7DA8A203-25BA-40C0-B4D8-F80CEE55D7AE");
			types[typeidof(uint16_t)].SetArrayConstructor<uint16_t>();

			REGISTER_NATIVE_TYPE(int16_t, "2592B6AE-2F6D-4FBD-A8F5-354818CFBF56");
			REGISTER_NATIVE_TYPE(int16_t*, "8C43F949-05B7-489E-B735-6F11D89C1AAC");
			REGISTER_NATIVE_TYPE(const int16_t*, "BEFF6CF9-E87D-419A-B14C-18595418B13B");
			types[typeidof(int16_t)].SetArrayConstructor<int16_t>();

			auto& stringType = types[typeidof(GuGuUtf8Str)];

			//:显式地为字符串添加构造函数
			stringType.AddConstructor<GuGuUtf8Str, false, false>({});
			stringType.AddConstructor<GuGuUtf8Str, false, true>({});
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

		void ReflectionDatabase::registerGuid(const GGuid& guid, int32_t id)
		{
			auto search = guids.find(guid);

			//已经定义了
			if (search != guids.end())
				return;

			guids[guid] = id;
		}

		void ReflectionDatabase::checkHaveSameTypeGuid()
		{
			for (int32_t i = 0; i < types.size(); ++i)
			{
				for (int32_t j = 0; j < types.size(); ++j)
				{
					if (i != j)
					{
						if (types[i].typeGuid == types[j].typeGuid)
						{
							GuGu_LOGE("%s have same type with %s", types[i].name.getStr(), types[j].name.getStr());
						}
					}
				}
			}
		}

	}
}

