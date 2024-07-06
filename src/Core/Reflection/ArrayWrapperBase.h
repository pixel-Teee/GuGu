#pragma once

#include "Array.h"

namespace GuGu {
	namespace meta {
		class Variant;
		class ArrayWrapperBase
		{
		public:
			virtual ~ArrayWrapperBase(void) {}

			virtual Variant GetValue(size_t index) = 0;
		};
	}
}