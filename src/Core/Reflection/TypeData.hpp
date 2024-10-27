#include "TypeData.h"
#pragma once

namespace GuGu {
	namespace meta {
		template<typename ClassType>
		void TypeData::SetArrayConstructor(void)
		{
			arrayConstructor = Constructor{
				typeof(ClassType),
				{},
				new ConstructorInvoker<Array<ClassType>, false, false>(),
				false
			};
		}
		template<typename ClassType, bool IsDynamic, bool IsWrapped, typename ...Args>
		void TypeData::AddConstructor()
		{
			InvokableSignature signature =
				Invokable::CreateSignature<Args...>();

			Constructor ctor{
				typeof(ClassType),
				signature,
				new ConstructorInvoker<ClassType, IsDynamic, IsWrapped, Args...>(), //dynamic 表示构造出来的对象是堆上的，否则是栈上的
				IsDynamic
			};

			//ctor.m_meta = meta;

			if (IsDynamic)
				dynamicConstructors.emplace(signature, ctor);
			else
				constructors.emplace(signature, ctor);
		}
		template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
		void TypeData::AddField(
			const std::string& name,
			GetterReturnType(ClassType::* methodGetter)(void),
			void (ClassType::* methodSetter)(SetterArgumentType)
		)
		{
			typedef FieldGetter<ClassType, GetterReturnType, true> GetterType;
			typedef FieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);
		}

		///////////////////////////////////////////////////////////////////////

		template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
		void TypeData::AddField(
			const std::string& name,
			GetterReturnType(ClassType::* methodGetter)(void) const,
			void (ClassType::* methodSetter)(SetterArgumentType)
		)
		{
			typedef FieldGetter<ClassType, GetterReturnType, true> GetterType;
			typedef FieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);
		}

		template<typename ClassType, typename FieldType, typename GetterReturnType>
		void TypeData::AddField(
			const std::string& name,
			GetterReturnType(ClassType::* methodGetter)(void),
			typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
		)
		{
			typedef FieldGetter<ClassType, GetterReturnType, true> GetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!fieldSetter ? nullptr : new FieldSetter<ClassType, FieldType, false>(fieldSetter)
			);

		}

		///////////////////////////////////////////////////////////////////////

		template<typename ClassType, typename FieldType, typename GetterReturnType>
		void TypeData::AddField(
			const std::string& name,
			GetterReturnType(ClassType::* methodGetter)(void) const,
			typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
		)
		{
			typedef FieldGetter<ClassType, GetterReturnType, true> GetterType;

			static_assert(std::is_same<typename std::decay<GetterReturnType>::type, typename std::decay<FieldType>::type>::value,
				"Return type of getter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!methodGetter ? nullptr : new GetterType(methodGetter),
				!fieldSetter ? nullptr : new FieldSetter<ClassType, FieldType, false>(fieldSetter)
			);

		}

		///////////////////////////////////////////////////////////////////////

		template<typename ClassType, typename FieldType, typename SetterArgumentType>
		void TypeData::AddField(
			const std::string& name,
			typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
			void (ClassType::* methodSetter)(SetterArgumentType)
		)
		{
			typedef FieldSetter<ClassType, SetterArgumentType, true> SetterType;

			static_assert(std::is_same<typename std::decay<SetterArgumentType>::type, typename std::decay<FieldType>::type>::value,
				"Argument type of setter does not match field type. "
				"This results in undefined behavior! (Even if there exists a conversion constructor between the types)"
				);

			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!fieldGetter ? nullptr : new FieldGetter<ClassType, FieldType, false>(fieldGetter),
				!methodSetter ? nullptr : new SetterType(methodSetter)
			);

		}

		///////////////////////////////////////////////////////////////////////

		template<typename ClassType, typename FieldType>
		void TypeData::AddField(
			const std::string& name,
			typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
			typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
		)
		{
			fields.emplace_back(
				name,
				typeof(FieldType),
				typeof(ClassType),
				!fieldGetter ? nullptr : new FieldGetter<ClassType, FieldType, false>(fieldGetter),
				!fieldSetter ? nullptr : new FieldSetter<ClassType, FieldType, false>(fieldSetter)
			);

		}

	}
}