#pragma once

#include "FieldSetterBase.h"

namespace GuGu {
	namespace meta {
		template<typename ClassType, typename FieldType, bool IsMethod>
		class FieldSetter : public FieldSetterBase
		{
		public:
			void SetValue(Variant& obj, const Variant& value) override;
		};
	}
}

#include "FieldSetter.hpp"