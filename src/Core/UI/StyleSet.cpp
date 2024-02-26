#include <pch.h>

#include "StyleSet.h"
#include "Brush.h"

namespace GuGu {
	StyleSet::StyleSet()
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
		std::shared_ptr<Brush> checkerBoard = std::make_shared<Brush>();
		checkerBoard->m_tiling = true;
		checkerBoard->m_texturePath = u8"asset/CheckerBoard.png";
		m_brushes.insert({ u8"CheckerBoard", checkerBoard });

		std::shared_ptr<Brush> fun = std::make_shared<Brush>();
		fun->m_tiling = false;
		fun->m_texturePath = u8"asset/fun.jpg";
		m_brushes.insert({ u8"fun", fun });

		std::shared_ptr<Brush> biscuit = std::make_shared<Brush>();
		biscuit->m_tiling = false;
		biscuit->m_texturePath = u8"asset/biscuit.jpg";
		m_brushes.insert({ u8"biscuit", biscuit });
        
		std::shared_ptr<TextInfo> textInfo = std::make_shared<TextInfo>();
		textInfo->m_size = 12;//12pt
		textInfo->m_textPath = u8"asset/NotoSansSC-Regular.ttf";
		textInfo->m_name = u8"NotoSansSC-Regular.ttf";
		std::shared_ptr<TextBlockStyle> textBlockStyle = std::make_shared<TextBlockStyle>(textInfo, math::float4(0.59f, 0.98f, 0.59f, 1.0f), math::float2(1.0f, 0.0f), math::float4(0.0f, 1.0f, 0.0f, 0.4f));
		m_styles.insert({ "normalText", textBlockStyle });

	}
	StyleSet::~StyleSet()
	{
	}
	std::shared_ptr<Brush> StyleSet::getBrush(const GuGuUtf8Str& str)
	{
		auto it = m_brushes.find(str);
		if (it != m_brushes.end())
			return it->second;

		GuGu_LOGE("don't find brush!");

		return nullptr;
	}
	void StyleSet::getBrush(std::vector<std::shared_ptr<Brush>>& brush)
	{
		for (auto& it : m_brushes)
		{
			brush.push_back(it.second);
		}
	}
	std::shared_ptr<StyleSet> StyleSet::getStyle()
	{
		static std::shared_ptr<StyleSet> styleInsance = std::make_shared<StyleSet>();
		return styleInsance;
	}
	void StyleSet::clear()
	{
		m_brushes.clear();
		m_styles.clear();
	}
}