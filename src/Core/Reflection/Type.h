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
		private:
			//一个无符号整数
			TypeID m_id;

			bool m_isArray;
		};
	}
}