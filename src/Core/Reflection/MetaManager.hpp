#pragma once

namespace GuGu {
	namespace meta {
		template<typename PropertyType>
		const PropertyType* MetaManager::GetProperty(void) const
		{
			static_assert(std::is_base_of<MetaProperty, PropertyType>::value,
				"Type must be a MetaProperty."
				);

			//获取一个 meta::type 的实例
			static const auto type = typeof(PropertyType);

			auto search = m_properties.find(type);

			//不存在
			if (search == m_properties.end())
				return nullptr;

			return static_cast<const PropertyType*>(search->second);
		}
	}
}