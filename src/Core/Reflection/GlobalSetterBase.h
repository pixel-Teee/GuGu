#pragma once

namespace GuGu
{
	namespace meta
	{
		class Argument;

		class GlobalSetterBase
		{
		public:
			virtual ~GlobalSetterBase(void) { }

			virtual void SetValue(const Argument& value) = 0;
		};
	}
}