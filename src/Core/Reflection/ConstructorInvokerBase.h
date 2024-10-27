#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace GuGu {
	namespace meta {
		class Variant;
		class Argument;

		class ConstructorInvokerBase
		{
		public:
			virtual ~ConstructorInvokerBase(void) {}

			virtual Variant Invoke(const ArgumentList& arguments) = 0;
		};
	}
}