#pragma once

#if defined(COMPILER_CLANG) || defined(COMPILER_GNU)

#define IsTriviallyDefaultConstructible(x) std::has_trivial_default_constructor<x>::value

#else

#define IsTriviallyDefaultConstructible(x) std::is_trivially_default_constructible<x>::value

#endif

namespace GuGu {
	namespace meta {
		struct TypeData;

		//这个用来构造TypeData，以及特化一个类型的TypeID
		template<typename T>
		struct TypeInfo
		{
			static bool Defined;

			static void Register(TypeID id, TypeData& data, bool beingDefined, const GuGuUtf8Str& typeGuid);

		private:
			template<typename U = T>
			static void addDefaultConstructor(
				TypeData& data,
				typename std::enable_if<
				!IsTriviallyDefaultConstructible(U)
				>::type* = nullptr
			);

			template<typename U = T>
			static void addDefaultConstructor(
				TypeData& data,
				typename std::enable_if<
				IsTriviallyDefaultConstructible(U)
				>::type* = nullptr
			);
		};
	}
}

#include "TypeInfo.hpp"