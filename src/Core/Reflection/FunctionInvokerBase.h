#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace GuGu {
	namespace meta {
		class Variant;

		class FunctionInvokerBase
		{
		public:
			virtual ~FunctionInvokerBase() {}

			virtual Variant Invoke(const ArgumentList& arguments) = 0;
		};
	}
}