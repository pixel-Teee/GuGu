#pragma once

#include <Core/Reflection/MetaProperty.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	namespace meta {
		class DisplayName : public MetaProperty
		{
		public:
			DisplayName(const char* inDisplayName);

			GuGuUtf8Str m_displayName;
		};
	}
}