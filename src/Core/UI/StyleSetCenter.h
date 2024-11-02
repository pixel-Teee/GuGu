#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include "StyleSet.h"

namespace GuGu {
	class StyleSetCenter
	{
	public:
		static void RegisterStyleSet(const GuGuUtf8Str& styleSetName, std::shared_ptr<StyleSet> inStyleSet);

		static std::vector<std::shared_ptr<StyleSet>> GetStyles();

		static std::shared_ptr<StyleSet> GetStyle(const GuGuUtf8Str& styleSetName);
	private:
		static std::unordered_map<GuGuUtf8Str, std::shared_ptr<StyleSet>> m_styles;
	};
}