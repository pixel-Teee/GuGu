#include <pch.h>

#include "Brush.h"
#include "StyleSet.h"

namespace GuGu {
	StyleSet::StyleSet()
	{
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
	std::shared_ptr<Brush> StyleSet::getNoBrush()
	{
		return std::make_shared<Brush>();
	}
	void StyleSet::getBrush(std::vector<std::shared_ptr<Brush>>& brush)
	{
		for (auto& it : m_brushes)
		{
			brush.push_back(it.second);
		}
	}
	math::float4 StyleSet::getColor(const GuGuUtf8Str& colorName)
	{
		auto it = m_colors.find(colorName);
		if (it != m_colors.end())
			return it->second;

		GuGu_LOGE("don't find color!");

		return math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void StyleSet::setColor(const GuGuUtf8Str& colorName, Color inColor)
	{
		m_colors[colorName] = inColor.toFloat4();
	}

	//std::shared_ptr<StyleSet> StyleSet::getStyle()
	//{
	//	static std::shared_ptr<StyleSet> styleInsance = std::make_shared<StyleSet>();
	//	return styleInsance;
	//}
	void StyleSet::clear()
	{
		m_brushes.clear();
		m_styles.clear();
		m_colors.clear();
	}
}