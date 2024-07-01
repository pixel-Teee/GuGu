#pragma once

namespace GuGu {
	namespace meta {
		struct TypeData;

		template<typename T>
		struct TypeInfo
		{
			static bool Defined;

			static void Register(TypeID id, TypeData& data, bool beingDefined);

		private:

		};
	}
}

#include "TypeInfo.hpp"