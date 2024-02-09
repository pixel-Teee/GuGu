#include <pch.h>

#include "Style.h"

namespace GuGu {
	Style::Style()
	{
		
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
}