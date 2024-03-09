#pragma once

#include "nvrhi.h"

#include <unordered_map>

namespace GuGu {
	class BindingCache
	{
	public:
		BindingCache(nvrhi::IDevice* device)
			: m_device(device)
		{}

		nvrhi::BindingSetHandle GetCachedBindingSet(const nvrhi::BindingSetDesc& desc, nvrhi::IBindingLayout* layout);

		nvrhi::BindingSetHandle GetOrCreateBindingSet(const nvrhi::BindingSetDesc& desc, nvrhi::IBindingLayout* layout);

		void Clear();
	private:
		nvrhi::DeviceHandle m_device;
		std::unordered_map<size_t, nvrhi::BindingSetHandle> m_bindingSets;
	};
}