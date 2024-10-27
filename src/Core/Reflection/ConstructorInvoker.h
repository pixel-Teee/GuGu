#pragma once

#include "ConstructorInvokerBase.h"

namespace GuGu {
	namespace meta {
		class Variant;
		class Argument;

		template<typename ClassType, bool IsDynamic, bool IsObjectWrapped, typename ...ArgTypes>
		class ConstructorInvoker : public ConstructorInvokerBase
		{
		public:
			static_assert(THIS_ARG_COUNT <= MaxArgumentCount,
				"Constructor has too many arguments. It's time to generate more overloads."
				);

			Variant Invoke(const ArgumentList& arguments) override;
		};
	}
}

#include "ConstructorInvoker.hpp"