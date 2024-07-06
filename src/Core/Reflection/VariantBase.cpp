#include <pch.h>

#include "VariantBase.h"
#include "ArrayWrapper.h"

namespace GuGu {
	namespace meta {
		bool VariantBase::IsArray(void) const
		{
			return false;
		}

		ArrayWrapper VariantBase::GetArray(void) const
		{
			//invalid wrapper
			return {};
		}
	}
}