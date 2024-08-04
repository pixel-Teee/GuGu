#include "Argument.h"
#pragma once

namespace GuGu {
	namespace meta {
		template<typename T>
		Argument::Argument(const T& data)
			: m_typeID(typeidof(T))
			, m_isArray(meta_traits::IsArray<T>::value)
			, m_data(reinterpret_cast<const void*>(std::addressof(data)))
		{
			static_assert(!std::is_same<Argument, T>::value,
				"cannot use argument as an argument");
		}

		template<typename T>
		Argument::Argument(T& data)
			: m_typeID(typeidof(T))
			, m_isArray(meta_traits::IsArray<T>::value)
			, m_data(reinterpret_cast<const void*>(std::addressof(data)))
		{
            static_assert(!std::is_same<Argument, T>::value,
                          "cannot use argument as an argument");
		}

		template<typename T>
		T& meta::Argument::GetValue(void) const
		{
			return *reinterpret_cast<typename std::remove_reference<T>::type*>
				(
					const_cast<void*>(m_data)
				);
		}
	}
}