#include <pch.h>

#include "Type.h"
#include "ReflectionDatabase.h"

namespace GuGu {
	namespace meta {
		namespace
		{
			//确认我们总是有 gDatabase 的一个引用
#define gDatabase ReflectionDatabase::Instance()
		}
		Type::Type(void)
			: m_id(InvalidTypeID)
			, m_isArray(false) 
		{
		}
		Type::Type(const Type& rhs)
			: m_id(rhs.m_id)
			, m_isArray(rhs.m_isArray)
		{
		}
		Type::Type(TypeID id, bool isArray)
			: m_id(id)
			, m_isArray(isArray)
		{
		}
		Type::operator bool(void) const
		{
			return m_id != InvalidTypeID;
		}
		bool Type::operator<(const Type& rhs) const
		{
			return m_id < rhs.m_id;
		}
		bool Type::operator>(const Type& rhs) const
		{
			return m_id > rhs.m_id;
		}
		bool Type::operator<=(const Type& rhs) const
		{
			return m_id <= rhs.m_id;
		}
		bool Type::operator>=(const Type& rhs) const
		{
			return m_id >= rhs.m_id;
		}
		bool Type::operator==(const Type& rhs) const
		{
			return m_id == rhs.m_id;
		}
		bool Type::operator!=(const Type& rhs) const
		{
			return m_id != rhs.m_id;
		}
		const Type& Type::Invalid(void)
		{
			static const Type invalid{ InvalidTypeID };
			return invalid;
		}
		TypeID Type::GetID(void) const
		{
			return m_id;
		}
		Type::List Type::GetTypes(void)
		{
			auto count = gDatabase.types.size();//vector<TypeData>

			List types;//vector<Type>

			for (TypeID i = 1; i < count; ++i)
			{
				types.emplace_back(i);
			}

			return types;
		}
	}
}


