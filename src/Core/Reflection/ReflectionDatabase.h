#pragma once

#include <vector>
#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include "TypeID.h"
#include "TypeData.h"

namespace GuGu {
	namespace meta
	{
		class ReflectionDatabase
		{
		public:
			ReflectionDatabase(void);

			~ReflectionDatabase();

			//第一个类型是invalid type id
			std::vector<TypeData> types;//关于一个类型具体的类型数据，是否为枚举，是否为指针，字段数组等

			std::unordered_map<GuGuUtf8Str, TypeID> ids;//字符串对应的TypeID

			std::unordered_map<GGuid, TypeID> guids;//guid对应的TypeID

			//加入全局变量

			//加入全局函数

			static ReflectionDatabase& Instance(void);

			TypeID AllocateType(const GuGuUtf8Str& name);

			void registerGuid(const GGuid& guid, int32_t id);

			void checkHaveSameTypeGuid();
		private:
			TypeID m_nextID;
		};
	}
}