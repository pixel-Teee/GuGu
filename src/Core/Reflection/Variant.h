#pragma once

#include "Object.h"
#include "VariantPolicy.h"
#include "VariantBase.h"
#include "Argument.h"

#include "json.hpp" //json

#define DISABLE_VARIANT					 \
	typename std::enable_if<			 \
		!std::is_same<Variant, T>::value \
	>::type*							 \

#define DISABLE_VARIANT_DECL DISABLE_VARIANT = nullptr

#define DISABLE_ARGUMENT				  \
	typename std::enable_if<			  \
		!std::is_same<Argument, T>::value \
	>::type*							  \

#define DISABLE_ARGUMENT_DECL DISABLE_ARGUMENT = nullptr

#define DISABLE_CONST		 \
	typename std::enable_if< \
	!std::is_const<T>::value \
	>::type*				 \

#define DISABLE_CONST_DECL DISABLE_CONST = nullptr

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

			template<typename T>
			Variant(T& data, DISABLE_VARIANT_DECL);

			template<typename T>
			Variant(T& data, variant_policy::NoCopy, DISABLE_VARIANT_DECL);

			//非常量右值引用，排除其它的 variants 和 arguments
			template<typename T>
			Variant(T&& data, DISABLE_VARIANT_DECL, DISABLE_ARGUMENT_DECL, DISABLE_CONST_DECL);

			//非常量数组类型
			template<typename T>
			Variant(Array<T>& rhs);

			//常量数组类型
			template<typename T>
			Variant(const Array<T>& rhs);

			//右值数组类型(非常量)
			template<typename T>
			Variant(Array<T>&& rhs);

			//右值数组类型(常量)
			template<typename T>
			Variant(const Array<T>&& rhs);

			//拷贝构造
			Variant(const Variant& rhs);

			//右值
			Variant(Variant&& rhs) noexcept;

			~Variant(void);

			template<typename T>
			Variant& operator=(T&& rhs);

			Variant& operator=(const Variant& rhs);

			Variant& operator=(Variant&& rhs);

			operator bool(void) const;

			Type GetType(void) const;
			ArrayWrapper GetArray(void) const;

			void Swap(Variant& other);

			int ToInt(void) const;
			bool ToBool(void) const;
			float ToFloat(void) const;
			double ToDouble(void) const;
			GuGuUtf8Str ToString(void) const;

			//todo:添加 serialize json
			nlohmann::json SerializeJson(void) const;

			template<typename T>
			T& GetValue(void) const;

			bool IsValid(void) const;
			bool IsConst(void) const;
			bool IsArray(void) const;

			meta::VariantBase* getBase() const;
		private:
			friend class Type;
			friend class Argument;
			friend class Destructor;
			bool m_isConst;

			void* getPtr(void) const;

			VariantBase* m_base;
		};
	}
}

#include "Variant.hpp"

#undef DISABLE_VARIANT
#undef DISABLE_VARIANT_DECL
#undef DISABLE_ARGUMENT
#undef DISABLE_ARGUMENT_DECL
#undef DISABLE_CONST
#undef DISABLE_CONST_DECL