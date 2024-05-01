#include <pch.h>

#include "Style.h"
#include "Brush.h"

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
	EditableTextBoxStyle::~EditableTextBoxStyle()
	{
	}
	SpinBoxStyle::~SpinBoxStyle()
	{
	}
	ScrollBarStyle::ScrollBarStyle()
		: m_horizontalBackgroundImage()
		, m_verticalBackgroundImage()
		, m_verticalTopSlotImage()
		, m_horizontalTopSlotImage()
		, m_verticalBottomSlotImage()
		, m_horizontalBottomSlotImage()
		, m_normalThumbImage()
		, m_thickNess(16.0f)
	{
		std::shared_ptr<Brush> noResource = std::make_shared<Brush>();
		noResource->m_tiling = false;
		noResource->m_texturePath = "";
		noResource->m_drawAs = BrushDrawType::NoDrawType;
		noResource->m_actualSize = math::int2(0, 0);
		noResource->m_margin = Padding(0.0f, 0.0f, 0.0f, 0.0f);

		m_horizontalBackgroundImage = noResource;
		m_verticalBackgroundImage = noResource;
		m_verticalTopSlotImage = noResource;
		m_horizontalTopSlotImage = noResource;
		m_verticalBottomSlotImage = noResource;
		m_horizontalBottomSlotImage = noResource;
		m_normalThumbImage = noResource;
	}
	ScrollBarStyle::~ScrollBarStyle()
	{
	}
	ComboButtonStyle::ComboButtonStyle()
	{
	}
	TableRowStyle::TableRowStyle()
	{
	}
	TableRowStyle::~TableRowStyle()
	{
	}
	ComboBoxStyle::ComboBoxStyle()
	{
	}
	ComboBoxStyle::~ComboBoxStyle()
	{
	}
}