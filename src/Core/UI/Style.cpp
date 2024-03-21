#include <pch.h>

#include "Style.h"

namespace GuGu {
	Style::Style()
	{
	}
	Style::~Style()
	{
	}
	TextBlockStyle::TextBlockStyle(std::shared_ptr<TextInfo> textInfo, math::float4 textColor, math::float2 shadowOffset, math::float4 shadowColor)
		: m_textInfo(textInfo)
		, m_textColor(textColor)
		, m_shadowOffset(shadowOffset)
		, m_shadowColor(shadowColor)
	{
	}
	TextBlockStyle::~TextBlockStyle()
	{
	}
	ButtonStyle::~ButtonStyle()
	{
	}
	CheckBoxStyle::~CheckBoxStyle()
	{
	}
	SliderStyle::~SliderStyle()
	{
	}
}