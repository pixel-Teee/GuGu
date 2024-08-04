#pragma once

#include "Type.h"

#include "Variant.h"

namespace GuGu {
	namespace meta {
		template<typename T>
		Type Type::Get(T&& obj) //根据一个类型，返回一个Type
		{
			return {typeof(T)};
		}

		template<typename ClassType>
		nlohmann::json Type::SerializeJson(const ClassType& instance, bool invokeHook)
		{
			auto type = typeof(ClassType);//获取Type对象

			assert(type.Invalid());

			Variant variant = instance;//从instance构造variant

			return type.SerializeJson(variant, invokeHook);
		}
	}
}