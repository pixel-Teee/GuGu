#pragma once

namespace GuGu {
	template<typename KeyType, typename ValueType>
	class Map : public std::map<KeyType, ValueType>
	{
	public:
		Map(void);
		Map(const std::map<KeyType, ValueType>& rhs);
		Map(const std::map<KeyType, ValueType>&& rhs);
	};
}

#include "Map.hpp"