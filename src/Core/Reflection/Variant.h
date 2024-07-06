#pragma once

#include "Object.h"
#include "VariantPolicy.h"

namespace GuGu {
	namespace meta {
		class Variant
		{
		public:
			Variant(void);

			//给定一个类型，从 meta::Object 派生，并且被 wrap object policy 供给，使用 object wrapper variant base
			//第三个参数单纯用来判断T从Object派生的，不需要填入
			template<typename T>
			Variant(T* data, variant_policy::WrapObject, typename std::enable_if<std::is_base_of<Object, T>::value>::type* = nullptr);

		private:
			bool m_isConst;
		};
	}
}