#include <pch.h>

#include "DisplayName.h"

namespace GuGu {

	namespace meta {
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

		GuGu::meta::Object* DisplayName::Clone(void) const
		{
			//throw std::logic_error("The method or operation is not implemented.");
			DisplayName* newDisplayName = new DisplayName(*this);
			return newDisplayName;
		}

		void DisplayName::PostLoad()
		{
			//throw std::logic_error("The method or operation is not implemented.");
		}

	}

}