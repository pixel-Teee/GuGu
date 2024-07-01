#pragma once

#include "TypeID.h"

#include <vector>

namespace GuGu {
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
		private:
			//一个无符号整数
			TypeID m_id;
			//是否为数组
			bool m_isArray;
		};
	}
}