//#include "VariantContainer.h"
#pragma once

#include "Type.h"
#include "VariantContainer.h"

#define DEFAULT_TYPE_HANDLER_IMPL(typeName)														 \
	template<typename T>																		 \
	template<typename U>																		 \
	typeName VariantContainer<T>::get##typeName(												 \
		typename std::enable_if<																 \
			!std::is_convertible<typename meta_traits::TypeOrEnumType<U>::type, typeName>::value \
		>::type*																				 \
		) const																					 \
	{																							 \
		return typeName();																		 \
	}																							 \
	template<typename T>																		 \
	template<typename U>																		 \
	typeName VariantContainer<T>::get##typeName(												 \
		typename std::enable_if<																 \
			 std::is_convertible<typename meta_traits::TypeOrEnumType<U>::type, typeName>::value \
		>::type*																				 \
		) const																					 \
	{																							 \
		return	static_cast<typeName>(m_value);													 \
	}	


namespace GuGu {
	namespace meta {
		template<typename T>
		VariantContainer<T>::VariantContainer(const NonRefType& value)
			: m_value(const_cast<NonRefType&>(value))
		{}

		template<typename T>
		VariantContainer<T>::VariantContainer(const NonRefType&& value)
			: m_value(std::move(const_cast<NonRefType&&>(value)))
		{}

		template<typename T>
		Type VariantContainer<T>::GetType(void) const
		{
			return typeof(T);
		}

		template<typename T>
		int32_t VariantContainer<T>::ToInt(void) const
		{
			return getint();
		}

		template<typename T>
		bool VariantContainer<T>::ToBool(void) const
		{
			return getbool();
		}

		template<typename T>
		float VariantContainer<T>::ToFloat(void) const
		{
			return getfloat();
		}

		template<typename T>
		double VariantContainer<T>::ToDouble(void) const
		{
			return getdouble();
		}

		template<typename T>
		GuGuUtf8Str VariantContainer<T>::ToString(void) const
		{
			return getString();
		}

		template<typename T>
		void* VariantContainer<T>::GetPtr(void) const
		{
			return const_cast<void*>(
				reinterpret_cast<const void*>(
						std::addressof(m_value)
					)
				);
		}

		template<typename T>
		VariantBase* VariantContainer<T>::Clone(void) const
		{
			//使用非引用类型，保证拷贝起作用
			return new VariantContainer<NonRefType>(m_value);
		}

		template<typename T>
		inline void VariantContainer<T>::OnSerialize(nlohmann::json& output) const
		{
			onSerialize(output);
		}

		template<typename T>
		inline void VariantContainer<T>::OnDeserialize(const nlohmann::json& input)
		{
			onDeserialize(input);
		}

		DEFAULT_TYPE_HANDLER_IMPL(int);
		DEFAULT_TYPE_HANDLER_IMPL(bool);
		DEFAULT_TYPE_HANDLER_IMPL(float);
		DEFAULT_TYPE_HANDLER_IMPL(double);
		DEFAULT_TYPE_HANDLER_IMPL(uint8_t);

		template<typename T>
		template<typename U>
		inline void VariantContainer<T>::onSerialize(nlohmann::json& output,
			typename std::enable_if<
			!std::is_pointer<U>::value&& std::is_base_of<Object, U>::value
			>::type*
		) const
		{
			m_value.OnSerialize(output);
		}

		template<typename T>
		template<typename U>
		void VariantContainer<T>::onSerialize(
			nlohmann::json& output,
			typename std::enable_if<
			std::is_pointer<U>::value || !std::is_base_of<Object, U>::value
			>::type*
		) const
		{
			// do nothing
		}

		template<typename T>
		template<typename U>
		GuGuUtf8Str VariantContainer<T>::getString(
			typename std::enable_if<
			!std::is_arithmetic<U>::value
			>::type*
			) const
		{
			return GuGuUtf8Str();
		}

		template<typename T>
		template<typename U>
		GuGuUtf8Str VariantContainer<T>::getString(
			typename std::enable_if<
			std::is_arithmetic<U>::value //是int或者float类型
			>::type*
		) const
		{
			return GuGuUtf8Str(std::to_string(m_value));
		}

		template<typename T>
		template<typename U>
		void VariantContainer<T>::onDeserialize(
			const nlohmann::json& input,
			typename std::enable_if<
			!std::is_pointer<U>::value&& std::is_base_of<Object, U>::value
			>::type*
		)
		{
			m_value.OnDeserialize(input);
		}

		template<typename T>
		template<typename U>
		void VariantContainer<T>::onDeserialize(
			const nlohmann::json& input,
			typename std::enable_if<
			std::is_pointer<U>::value || !std::is_base_of<Object, U>::value
			>::type*
		)
		{
			// do nothing
		}
	}
}

#undef DEFAULT_TYPE_HANDLER_IMPL