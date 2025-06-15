#pragma once

#include "ArrayWrapperBase.h"

namespace GuGu {
	namespace meta {
		class Argument;

		class ArrayWrapper
		{
		public:
			ArrayWrapper(void);

			template<typename T>
			ArrayWrapper(Array<T>& rhs);

			template<typename T>
			ArrayWrapper(const Array<T>& rhs);

			Variant GetValue(size_t index) const;
			void SetValue(size_t index, const Argument& value);

			void Insert(size_t index, const Argument& value);
			void Remove(size_t index);

			size_t Size(void) const;

			bool IsValid(void) const;
			bool IsConst(void) const;

			void Resize(size_t newSize);
		private:
			bool m_isConst;

			ArrayWrapperBase* m_base;
		};
	}
}

#include "ArrayWrapper.hpp"