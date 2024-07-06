#include <pch.h>

#include "MetaManager.h"

namespace GuGu {
	namespace meta {
		MetaManager::MetaManager(const Initializer& properties)
		{
			for (auto& prop : properties)
			{
				//Invalid meta property registered, make sure the property type is enabled in reflection
				assert(prop.first.IsValid());

				//注册属性
				SetProperty(prop.first, prop.second);
			}
		}
		MetaManager::~MetaManager(void)
		{
		}
		void MetaManager::SetProperty(Type type, const MetaProperty* value)
		{
			auto search = m_properties.find(type);

			//删除老的属性，如果它存在
			if (search != m_properties.end())
				delete search->second;

			m_properties[type] = value;
		}
	}
}
