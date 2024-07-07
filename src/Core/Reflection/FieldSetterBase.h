#pragma once

namespace GuGu {
	namespace meta
	{
		class Variant;

		class FieldSetterBase
		{
		public:
			virtual ~FieldSetterBase(void) {}

			virtual void SetValue(Variant& obj, const Variant& value) = 0;
		};
	}
}