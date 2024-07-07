#pragma once

#include <vector>
#include <unordered_map>

#define THIS_ARG_COUNT sizeof...(ArgTypes)

namespace GuGu {
	namespace meta {
		class Type;

		typedef std::vector<Type> InvokableSignature;

		template<typename T>
		using InvokableOverloadMap = std::unordered_multimap<InvokableSignature, T>;

		//最大参数数量支持
		const size_t MaxArgumentCount = 10;
	}
}