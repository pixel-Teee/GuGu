#pragma once

#include "Variant.h"

namespace GuGu {
	namespace meta {
		class TypeCreator
		{
		public:
			template<typename... Args>
			static Variant Create(const Type& type, Args&& ...args);
		};
	}
}

#include "TypeCreator.hpp"