#pragma once

#include "Variant.h"
#include "Argument.h"

namespace GuGu {
	namespace meta {
		template<typename ...Args>
		Variant Constructor::Invoke(Args &&...args) const
		{
			ArgumentList arguments{ std::forward<Args>(args)... };

			return InvokeVariadic(arguments);
		}
	}
}