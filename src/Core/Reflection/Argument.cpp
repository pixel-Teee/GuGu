#include <pch.h>

#include "Argument.h"
#include "Variant.h"
#include "Type.h"

namespace GuGu {
	namespace meta {
		Argument::Argument(void)
			: m_typeID(InvalidTypeID)
			, m_isArray(false)
			, m_data(nullptr)
		{
		}
		Argument::Argument(const Argument& rhs)
			: m_typeID(rhs.m_typeID)
			, m_isArray(rhs.m_isArray)
			, m_data(rhs.m_data)
		{
		}
		Argument::Argument(Variant& obj)
			: m_typeID(obj.GetType().GetID())
			, m_isArray(obj.GetType().IsArray())
			, m_data(obj.getPtr())
		{
		}
		Argument::Argument(const Variant& obj)
			: m_typeID(obj.GetType().GetID())
			, m_isArray(obj.GetType().IsArray())
			, m_data(obj.getPtr())
		{
		}

		Argument::Argument(Argument& rhs)
			: m_typeID(rhs.m_typeID)
			, m_isArray(rhs.m_isArray)
			, m_data(rhs.m_data)
		{
		
		}

		Argument& Argument::operator=(const Argument& rhs)
		{
			m_data = rhs.m_data;

			const_cast<TypeID&>(m_typeID) = rhs.m_typeID;

			return *this;
		}
		Type Argument::GetType(void) const
		{
			return Type(m_typeID, m_isArray);
		}
		void* Argument::GetPtr(void) const
		{
			return const_cast<void*>(m_data);
		}
	}
}