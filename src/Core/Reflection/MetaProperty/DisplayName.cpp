#include <pch.h>

#include "DisplayName.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registermetaDisplayName()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("meta::DisplayName");
		auto& type = db.types[id];
		meta::TypeInfo<meta::DisplayName>::Register(id, type, true, "86AC4FFE-0C5A-46F2-BF40-AAE89636AE44");
		return true;
	}

	namespace meta {
		IMPLEMENT_INITIAL_BEGIN(DisplayName)
			ADD_INITIAL_FUNCTION_WITH_PRIORITY(registermetaDisplayName)
		IMPLEMENT_INITIAL_END

		DisplayName::DisplayName(const char* inDisplayName)
			: m_displayName(inDisplayName)
		{

		}

		DisplayName::DisplayName(const DisplayName& rhs)
		{
			m_displayName = rhs.m_displayName;
		}

		void DisplayName::Update(float fElapsedTimeSeconds)
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

		meta::Type DisplayName::GetType() const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return typeof(DisplayName);
		}

		meta::Object* DisplayName::Clone(void) const
		{
			DisplayName* newDisplayName = new DisplayName(*this);
			newDisplayName->m_displayName = m_displayName;
			return newDisplayName;
		}

		void DisplayName::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

	}
}