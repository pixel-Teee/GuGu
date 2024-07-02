#pragma once

#include <vector>

namespace GuGu {
	//std::vector 的 wrapper
	template<typename T>
	class Array : public std::vector<T>
	{
	public:
		Array(void);
		Array(const std::vector<T>& rhs);
		Array(const std::vector<T>&& rhs);
		Array(const std::initializer_list<T>& rhs);
		Array(const std::initializer_list<T>&& rhs);
	};
}

#include "Array.hpp"