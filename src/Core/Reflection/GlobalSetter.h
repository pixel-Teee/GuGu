#pragma once

#include "GlobalSetterBase.h"

namespace GuGu
{
	namespace meta
	{
		template<typename GlobalType, bool IsMethod>
		class GlobalSetter : public GlobalSetterBase
		{
		public:
			void SetValue(const Argument& value) override;
		};
	}
}

#include "GlobalSetter.hpp"