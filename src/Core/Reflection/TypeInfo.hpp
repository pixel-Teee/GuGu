#pragma once

#include "TypeData.h"
#include "TypeId.h"

namespace GuGu {
	namespace meta {
		template<typename T>
		bool TypeInfo<T>::Defined = false;

		//check is shared ptr
		template<typename>
		struct isSharedPtr : std::false_type {};
		template<typename T>
		struct isSharedPtr<std::shared_ptr<T>> : std::true_type { };

		template<typename>
		struct isWeakPtr : std::false_type {};
		template<typename T>
		struct isWeakPtr<std::weak_ptr<T>> : std::true_type { };

		template<typename T>
		void TypeInfo<T>::Register(TypeID id, TypeData& data, bool beingDefined)
		{
			//已经定义了
			if (id == Type::Invalid().GetID())
				return;

			TypeIDs<T>::ID = id;

			typedef typename std::remove_pointer<typename std::decay<T>::type>::type Decayed;

			data.isClass = std::is_class<Decayed>::value;
			data.isEnum = std::is_enum<Decayed>::value;
			data.isPointer = std::is_pointer<T>::value;
			data.isPrimitive = std::is_arithmetic<Decayed>::value;
			data.isFloatingPoint = std::is_floating_point<Decayed>::value;
			data.isSigned = std::is_signed<Decayed>::value;
			data.isSharedPointer = isSharedPtr<T>::value;
			data.isWeakPointer = isWeakPtr<T>::value;
			data.isStruct = std::is_standard_layout<T>::value && !data.isPrimitive && !data.isPointer && !data.isSharedPointer && !data.isWeakPointer;
			
			if (beingDefined)
			{
				//添加默认构造函数
				addDefaultConstructor(data);

				//应用平凡属性
			}
		}

		template<typename T>
		template<typename U>
		void TypeInfo<T>::addDefaultConstructor(
			TypeData& data,
			typename std::enable_if<
			!IsTriviallyDefaultConstructible(U)
			>::type*
		)
		{
			// do nothing
		}

		template<typename T>
		template<typename U>
		void TypeInfo<T>::addDefaultConstructor(
			TypeData& data,
			typename std::enable_if<
			IsTriviallyDefaultConstructible(U)
			>::type*
		)
		{
			// add the good 'ol default constructor
			data.AddConstructor<T, false, false>();

			// add the good 'ol dynamic default constructor
			data.AddConstructor<T, true, false>();
		}
	}
}