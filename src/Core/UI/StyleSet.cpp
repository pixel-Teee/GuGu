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
	//std::shared_ptr<StyleSet> StyleSet::getStyle()
	//{
	//	static std::shared_ptr<StyleSet> styleInsance = std::make_shared<StyleSet>();
	//	return styleInsance;
	//}
	void StyleSet::clear()
	{
		m_brushes.clear();
		m_styles.clear();
	}
}