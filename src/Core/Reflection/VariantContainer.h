#pragma once

#include "VariantBase.h"

namespace GuGu {
	namespace meta {
		template<typename T>
		class VariantContainer : public VariantBase
		{
		public:
			typedef typename std::remove_reference<T>::type NonRefType;
		};
	}
}