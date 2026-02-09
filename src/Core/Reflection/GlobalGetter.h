#pragma once

#include "GlobalGetterBase.h"

namespace GuGu
{
	namespace meta
	{
		template<typename GlobalType, bool IsMethod>
		class GlobalGetter : public GlobalGetterBase
		{
		public:
			Variant GetValue(void) override;
		};
	}
}

#include "GlobalGetter.hpp"