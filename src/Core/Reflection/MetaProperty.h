#pragma once

#include "Object.h"

namespace GuGu {
	namespace meta {
		class MetaProperty : public Object
		{

		};

		template<typename PropertyType, typename... Args>
		MetaProperty* MetaPropertyInitializer(Args&&... args);//构造一个PropertyType，传入参数到 PropertyType 的构造函数
	}
}

#include "MetaProperty.hpp"