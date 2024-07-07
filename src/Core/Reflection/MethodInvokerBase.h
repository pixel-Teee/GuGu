#pragma once

#include "InvokableConfig.h"
#include "ArgumentConfig.h"

namespace GuGu {
	namespace meta {
		class Variant;

		class MethodInvokerBase
		{
		public:
			virtual ~MethodInvokerBase(void) {}

			virtual Variant Invoke(Variant& obj, const ArgumentList& arguments) = 0;
		};
	}
}