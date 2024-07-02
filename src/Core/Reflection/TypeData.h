#pragma once

#include "Type.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	
	namespace meta
	{
		class ReflectionDatabase;

		struct TypeData
		{
			bool isEnum : 1; //是否为枚举类型
			bool isPrimitive : 1; //是否为原子类型
			bool isSigned : 1; //是否有符号
			bool isFloatingPoint : 1; //是否为浮点数
			bool isPointer : 1; //是否为指针
			bool isClass : 1; //是否为类

			//todo:添加 meta 管理器

			GuGuUtf8Str name;

			//enum type
			//Enum enumeration;

			//class type
			Type::Set baseClasses; //基类
			Type::Set derivedClasses; //派生类

			//构造函数

			//析构函数

			//字段

			//静态字段

			//方法

			//静态方法

			TypeData(void);
			TypeData(const GuGuUtf8Str& name);
		};
	}
}