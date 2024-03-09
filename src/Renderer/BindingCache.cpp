#include <pch.h>

#include "BindingCache.h"

namespace GuGu {
	nvrhi::BindingSetHandle BindingCache::GetCachedBindingSet(const nvrhi::BindingSetDesc& desc, nvrhi::IBindingLayout* layout)
	{
		size_t hash = 0;
		nvrhi::hash_combine(hash, desc);
		nvrhi::hash_combine(hash, layout);

		nvrhi::BindingSetHandle result = nullptr;
		auto it = m_bindingSets.find(hash);
		if (it != m_bindingSets.end())
			result = it->second;

		if (result)
		{
			assert(result->getDesc());
			assert(*result->getDesc() == desc);
		}

		return result;
	}
	nvrhi::BindingSetHandle BindingCache::GetOrCreateBindingSet(const nvrhi::BindingSetDesc& desc, nvrhi::IBindingLayout* layout)
	{
		size_t hash = 0;
		nvrhi::hash_combine(hash, desc);
		nvrhi::hash_combine(hash, layout);

		nvrhi::BindingSetHandle result;
		auto it = m_bindingSets.find(hash);
		if (it != m_bindingSets.end())
			result = it->second;

		if (!result)
		{

			nvrhi::BindingSetHandle& entry = m_bindingSets[hash];
			if (!entry)
			{
				result = m_device->createBindingSet(desc, layout);
				entry = result;
			}
			else
				result = entry;
		}

		if (result)
		{
			assert(result->getDesc());
			assert(*result->getDesc() == desc);
		}

		return result;
	}
	void BindingCache::Clear()
	{
		m_bindingSets.clear();
	}
}