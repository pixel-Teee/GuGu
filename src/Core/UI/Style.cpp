#include <pch.h>

#include "Style.h"
#include "Brush.h"

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
		std::shared_ptr<Brush> checkerBoard = std::make_shared<Brush>();
		checkerBoard->m_tiling = true;
		checkerBoard->m_texturePath = u8"CheckerBoard.png";
		m_styles.insert({ u8"CheckerBoard", checkerBoard });

		std::shared_ptr<Brush> fun = std::make_shared<Brush>();
		fun->m_tiling = false;
		fun->m_texturePath = u8"fun.jpg";
		m_styles.insert({ u8"fun", fun });

		std::shared_ptr<Brush> biscuit = std::make_shared<Brush>();
		biscuit->m_tiling = false;
		biscuit->m_texturePath = u8"biscuit.jpg";
		m_styles.insert({ u8"biscuit", biscuit });

		std::shared_ptr<TextInfo> textInfo = std::make_shared<TextInfo>();
		textInfo->m_size = 64;//12pt
		textInfo->m_textPath = u8"STKAITI.TTF";
		textInfo->m_name = u8"STKAITI.TTF";
		m_texts.insert({ u8"normalText", textInfo });
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
	std::shared_ptr<Style> Style::getStyle()
	{
		static std::shared_ptr<Style> styleInsance = std::make_shared<Style>();
		return styleInsance;
	}
	std::shared_ptr<TextInfo> Style::getTextInfo(const GuGuUtf8Str& str)
	{
		auto it = m_texts.find(str);
		if (it != m_texts.end())
			return it->second;

		GuGu_LOGE("don't find brush!");

		return nullptr;
	}
	void Style::clear()
	{
		m_styles.clear();
		m_texts.clear();
	}
}