#pragma once

#include "MetaTraits.h"
#include <type_traits>

//TypeIDs根据每个类型都特化，可以获取一个类型具体的ID
#define typeidof(expr)				 \
		GuGu::meta::TypeIDs<		 \
			GuGu::meta::CleanedType< \
				typename GuGu::meta_traits::RemoveArray<expr>::type \
				>													\
			>::ID													\
//转换类型到一个 meta::Type 实例
#define typeof(expr) \
		GuGu::meta::Type(  \
			typeidof(expr), \
				GuGu::meta_traits::IsArray<expr>::value \
		)	

//转换一个类型到一个 meta::Type 实例
#define decltypeof(expr) typeof(decltype(expr))
namespace GuGu {
	namespace meta {
		//去除各种限定符的类型
		template<typename T>
		using CleanedType = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
	}
}

