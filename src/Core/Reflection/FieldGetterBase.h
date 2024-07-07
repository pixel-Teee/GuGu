#pragma once

namespace GuGu {
	namespace meta
	{
		class Variant;

		class FieldGetterBase
		{
		public:
			virtual ~FieldGetterBase() {}

			virtual Variant GetValue(const Variant& obj) = 0;
			virtual Variant GetValueReference(const Variant& obj) = 0;
		};
	}
}