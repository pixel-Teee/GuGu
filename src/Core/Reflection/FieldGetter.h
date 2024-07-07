#pragma once

#include "FieldGetterBase.h"
#include "Variant.h"

namespace GuGu {
	namespace meta {
		template<typename ClassType, typename FieldType, bool IsMethod>
		class FieldGetter : public FieldGetterBase
		{
		public:
			Variant GetValue(const Variant& obj) override;
			Variant GetValueReference(const Variant& obj) override;
		};
	}
}

#include "FieldGetter.hpp"