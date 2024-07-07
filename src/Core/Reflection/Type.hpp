#pragma once

namespace GuGu {
	namespace meta {
		template<typename T>
		Type Type::Get(T&& obj) //根据一个类型，返回一个Type
		{
			return {typeof(T)};
		}
	}
}