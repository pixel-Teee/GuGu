#pragma once

#include "ArrayWrapperBase.h"

namespace GuGu {
	namespace meta {
		class Variant;
		class Argument;

		template<typename T>
		class ArrayWrapperContainer : public ArrayWrapperBase
		{
		public:
			ArrayWrapperContainer(Array<T>& a);

			Variant GetValue(size_t index) override;
			void SetValue(size_t index, const Argument& value) override;

			void Insert(size_t index, const Argument& value) override;
			void Remove(size_t index) override;

			size_t Size(void) const override;
		private:
			Array<T>& m_array;
		};
	}
}

#include "ArrayWrapperContainer.hpp"