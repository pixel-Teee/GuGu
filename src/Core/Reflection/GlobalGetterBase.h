#pragma once

namespace GuGu
{
	namespace meta
	{
		class Variant;

		class GlobalGetterBase
		{
		public:
			virtual ~GlobalGetterBase(void) { }

			virtual Variant GetValue(void) = 0;
		};
	}
}