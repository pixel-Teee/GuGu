#pragma once

#include "Array.h"

namespace GuGu {
	namespace meta {
		class Variant;
		class Argument;
		class ArrayWrapperBase
		{
		public:
			virtual ~ArrayWrapperBase(void) {}

			virtual Variant GetValue(size_t index) = 0;
			virtual void SetValue(size_t index, const Argument& value) = 0;

			virtual void Insert(size_t index, const Argument& value) = 0;
			virtual void Remove(size_t index) = 0;

			virtual size_t Size(void) const = 0;

			virtual void Resize(size_t newSize) = 0;
		};
	}
}