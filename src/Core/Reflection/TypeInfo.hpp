#pragma once

#include "TypeData.h"
#include "TypeId.h"

namespace GuGu {
	namespace meta {
		template<typename T>
		bool TypeInfo<T>::Defined = false;

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
			
			if (beingDefined)
			{
				//添加默认构造函数

				//应用平凡属性
			}
		}
	}
}