#include <pch.h>

#include "Type.h"

namespace GuGu {
	namespace meta {
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
	}
}


