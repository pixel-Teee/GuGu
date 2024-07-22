#pragma once

#include "TypeID.h"

#include "TypeConfig.h"

#include <vector>

#include <Core/GuGuUtf8Str.h>

#include "json.hpp" //json

namespace GuGu {
	//class GuGuUtf8Str;
	namespace meta
	{
		class Type
		{
		public:
			typedef std::vector<Type> List;
			typedef std::set<Type> Set;

			Type(void);
			Type(const Type& rhs);
			Type(TypeID id, bool isArray = false);

			operator bool(void) const; //如果是InvalidTypeID，返回false，否则返回true

			//判断m_id是否相等
			bool operator<(const Type& rhs) const;
			bool operator>(const Type& rhs) const;
			bool operator<=(const Type& rhs) const;
			bool operator>=(const Type& rhs) const;
			bool operator==(const Type& rhs) const;
			bool operator!=(const Type& rhs) const;

			//获取一个 invalid type 的实例
			static const Type& Invalid(void);

			//获取类型的内部 id
			TypeID GetID(void) const;

			//获取所有注册在主反射数据库的类型
			static List GetTypes(void);

			//获取一个类型，基于修饰的字符串名
			//参数是类型的名称
			static Type GetFromName(const GuGuUtf8Str& name);

			//通过推导object的类型来获取类型
			template<typename T>
			static Type Get(T&& obj);

			bool IsValid(void) const;

			bool IsPrimitive(void) const;

			bool IsFloatingPoint(void) const;//从TypeData查询类型数据

			bool IsSigned(void) const;

			bool IsEnum(void) const;

			bool IsPointer(void) const;

			bool IsClass(void) const;

			bool IsArray(void) const;

			//获取对于这个类型可读的名称
			GuGuUtf8Str GetName() const;

			Type GetDecayedType(void) const;
		private:
			//一个无符号整数
			TypeID m_id;
			//是否为数组
			bool m_isArray;
		};
	}
}

#include "Type.hpp"