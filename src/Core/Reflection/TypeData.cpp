#include <pch.h>

#include "TypeData.h"

namespace GuGu {
	namespace meta
	{
		TypeData::TypeData(void)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			//todo:添加enumeration的初始化
		{
		}
		TypeData::TypeData(const GuGuUtf8Str& name)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			, name(name)
			//todo:添加enumeration的初始化
		{
		}
	}
}