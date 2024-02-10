#include <pch.h>

#include "Style.h"

namespace GuGu {
	Style::Style()
	{
		//repeat sampler
		//auto defaultSamplerDesc = nvrhi::SamplerDesc()
		//	.setAllFilters(false)
		//	.setAllAddressModes(nvrhi::SamplerAddressMode::Clamp);
		//m_defaultSeampler = m_device->createSampler(defaultSamplerDesc);
		//
		//auto repeatSamplerDesc = nvrhi::SamplerDesc()
		//	.setAllFilters(false)
		//	.setAllAddressModes(nvrhi::SamplerAddressMode::Repeat);
		//m_repeatSeampler = m_device->createSampler(repeatSamplerDesc);
	}
	Style::~Style()
	{
	}
	std::shared_ptr<Brush> Style::getBrush(const GuGuUtf8Str& str)
	{
		auto it = m_styles.find(str);
		if (it != m_styles.end())
			return it->second;

		GuGu_LOGE("don't find brush!");

		return nullptr;
	}
	void Style::getBrush(std::vector<std::shared_ptr<Brush>>& brush)
	{
		for (auto& it : m_styles)
		{
			brush.push_back(it.second);
		}
	}
}