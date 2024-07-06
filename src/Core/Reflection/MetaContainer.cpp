#include <pch.h>

#include "MetaContainer.h"

namespace GuGu {
	namespace meta {
		const MetaManager& MetaContainer::GetMeta(void) const
		{
			return m_meta;
		}
	}
}
