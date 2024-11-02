#include <pch.h>

#include "CoreStyle.h"
#include "StyleSetCenter.h"

namespace GuGu {
	std::unordered_map<GuGuUtf8Str, std::shared_ptr<StyleSet>> StyleSetCenter::m_styles;

	void StyleSetCenter::RegisterStyleSet(const GuGuUtf8Str& styleSetName, std::shared_ptr<StyleSet> inStyleSet)
	{
		m_styles.insert({ styleSetName, inStyleSet });
	}
	std::vector<std::shared_ptr<StyleSet>> StyleSetCenter::GetStyles()
	{
		std::vector<std::shared_ptr<StyleSet>> styleSets;
		for (const auto& styleSet : m_styles)
		{
			styleSets.push_back(styleSet.second);
		}
		return styleSets;
	}
	std::shared_ptr<StyleSet> StyleSetCenter::GetStyle(const GuGuUtf8Str& styleSetName)
	{
		auto it = m_styles.find(styleSetName);
		if (it != m_styles.end())
		{
			return it->second;
		}
		return nullptr;
	}
}