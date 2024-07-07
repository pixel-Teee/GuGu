#include <pch.h>

#include "Variant.h"

namespace GuGu {
	namespace meta {
		Variant::Variant(void)
		{
		}
		void* Variant::getPtr(void) const
		{
			return m_base ? m_base->GetPtr() : nullptr;
		}
	}
}


