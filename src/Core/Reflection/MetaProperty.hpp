#pragma once

namespace GuGu {
	namespace meta {
		template<typename PropertyType, typename... Args>
		MetaProperty* MetaPropertyInitializer(Args&&... args)
		{
			//PropertyType 必须是 MetaProperty 的子类
			static_assert(std::is_base_of<MetaProperty, PropertyType>::value,
				"Meta properties must inherit from MetaProperty");

			return new PropertyType(std::forward<Args>(args)...);
		}
	}
}