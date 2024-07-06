#pragma once

namespace GuGu {
	namespace meta {
		struct TypeData;

		//这个用来构造TypeData，以及特化一个类型的TypeID
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