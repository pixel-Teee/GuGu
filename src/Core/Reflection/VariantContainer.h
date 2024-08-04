#pragma once

#include "VariantBase.h"

#include "MetaTraits.h"

//#include "Type.h"

#include "Object.h"

//子类能够向基类转换，typeName是基类
#define DEFAULT_TYPE_HANDLER(typeName)															\
	template<typename U = T>																	\
	typeName get##typeName(																		\
		typename std::enable_if<																\
			!std::is_convertible<typename meta_traits::TypeOrEnumType<U>::type, typeName>::value\
		>::type* = nullptr																		\
	) const;																					\
	template<typename U = T>																	\
	typeName get##typeName(																		\
		typename std::enable_if<																\
			std::is_convertible<typename meta_traits::TypeOrEnumType<U>::type, typeName>::value \
		>::type* = nullptr																		\
	) const;

namespace GuGu {
	namespace meta {
		template<typename T>
		class VariantContainer : public VariantBase
		{
		public:
			typedef typename std::remove_reference<T>::type NonRefType;

			VariantContainer(const NonRefType& value);
			VariantContainer(const NonRefType&& value);

			Type GetType(void) const override;
			void* GetPtr(void) const override;

			int32_t ToInt(void) const override;
			bool ToBool(void) const override;
			float ToFloat(void) const override;
			double ToDouble(void) const override;
			GuGuUtf8Str ToString(void) const override;

			VariantBase* Clone(void) const override;

			void OnSerialize(nlohmann::json& output) const override;

		private:
			friend class Variant;

			DEFAULT_TYPE_HANDLER(int);
			DEFAULT_TYPE_HANDLER(bool);
			DEFAULT_TYPE_HANDLER(float);
			DEFAULT_TYPE_HANDLER(double);

			template<typename U = T>
			GuGuUtf8Str getString(typename std::enable_if<!std::is_arithmetic<U>::value>::type* = nullptr) const;

			template<typename U = T>
			GuGuUtf8Str getString(typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr) const;

			template<typename U = T>
			void onSerialize(
				nlohmann::json& output,
				typename std::enable_if<
				!std::is_pointer<U>::value&& std::is_base_of<Object, U>::value
				>::type* = nullptr
			) const;

			template<typename U = T>
			void onSerialize(
				nlohmann::json& output,
				typename std::enable_if<
				std::is_pointer<U>::value || !std::is_base_of<Object, U>::value
				>::type* = nullptr
			) const;

			T m_value;

			VariantContainer& operator=(const VariantContainer& rhs) = delete;
		};
	}
}

#undef DEFAULT_TYPE_HANDLER

#include "VariantContainer.hpp"
#include "VariantContainerStandardTypes.hpp"