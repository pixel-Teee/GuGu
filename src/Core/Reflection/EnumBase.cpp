#include <pch.h>

#include "EnumBase.h"

#include "Variant.h"
#include "Argument.h"

namespace GuGu {
	namespace meta {
		EnumBase::EnumBase(const GuGuUtf8Str& name, TypeID owner)
			: m_parentType(owner)
			, m_name(name)
		{}

		Type EnumBase::GetParentType(void) const
		{
			return m_parentType;
		}

		const GuGuUtf8Str& EnumBase::GetName(void) const
		{
			return m_name;
		}
	}
}