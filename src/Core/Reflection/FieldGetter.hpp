#pragma once

namespace GuGu {
	namespace meta
	{
		//getter from method
		template<typename ClassType, typename ReturnType>
		class FieldGetter<ClassType, ReturnType, true> : public FieldGetterBase
		{
		public:
			typedef ReturnType(ClassType::* Signature)(void);
			typedef ReturnType(ClassType::* SignatureConst)(void) const;

			FieldGetter(Signature method)
				: m_method(reinterpret_cast<SignatureConst> (method)) {}

			FieldGetter(SignatureConst method)
				: m_method(method) {}

			Variant GetValue(const Variant& obj) override
			{
				auto& instance = obj.GetValue<ClassType>();

				return (instance.*m_method)();
			}

			Variant GetValueReference(const Variant& obj) override
			{
				return getValueReference(obj);
			}

		private:
			//从一个对象的方法获取一个值，从obj的方法获取值，放入Variant
			template<typename T = ReturnType>
			Variant getValueReference(
				const Variant& obj,
				typename std::enable_if<std::is_lvalue_reference<T>::value>::type* = nullptr
			)
			{
				auto& instance = obj.GetValue<ClassType>();

				return Variant{ (instance.*m_method)(), variant_policy::NoCopy() };
			}

			template<typename T = ReturnType>
			Variant getValueReference(const Variant& obj, typename std::enable_if<!std::is_lvalue_reference<T>::value>::type* = nullptr)
			{
				//非左值引用返回类型必须返回值
				return GetValue(obj);
			}
			SignatureConst m_method;
		};

		//getter from direct field
		template<typename ClassType, typename FieldType>
		class FieldGetter<ClassType, FieldType, false> : public FieldGetterBase
		{
		public:
			typedef FieldType(ClassType::* Signature);

			FieldGetter(Signature field)
				: m_field(field) { }

			Variant GetValue(const Variant& obj) override
			{
				auto& instance = obj.GetValue<ClassType>();

				return instance.*m_field;
			}

			Variant GetValueReference(const Variant& obj) override
			{
				auto& instance = obj.GetValue<ClassType>();

				return Variant{ instance.*m_field, variant_policy::NoCopy() };
			}

		private:
			Signature m_field;
		};
	}
}