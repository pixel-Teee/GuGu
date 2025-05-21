#include <pch.h>

#include "MetaManager.h"

namespace GuGu {
	namespace meta {
		MetaManager::MetaManager(void)
		{
		}
		MetaManager::MetaManager(const MetaManager& rhs)
		{
			copy(rhs);
		}

		MetaManager::MetaManager(const MetaManager&& rhs)
			: m_properties(std::move(rhs.m_properties))
		{

		}

		const MetaManager& MetaManager::operator=(const MetaManager& rhs)
		{
			copy(rhs);

			return *this;
		}
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
			for (auto& prop : m_properties)
				delete prop.second;
			//m_properties.clear();
		}

		meta::Variant MetaManager::GetProperty(Type type) const
		{
			auto search = m_properties.find(type);
			if (search == m_properties.end())
				return {};
			return ObjectVariant(search->second);
		}

		void MetaManager::SetProperty(Type type, const MetaProperty* value)
		{
			auto search = m_properties.find(type);

			//删除老的属性，如果它存在
			if (search != m_properties.end())
				delete search->second;

			m_properties[type] = value;
		}

		void MetaManager::copy(const MetaManager& rhs)
		{
			for (auto& prop : rhs.m_properties)
			{
				m_properties[prop.first] =
					static_cast<MetaProperty*>(prop.second->Clone());
			}
		}
	}
}
