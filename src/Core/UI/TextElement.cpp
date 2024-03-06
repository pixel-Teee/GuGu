#include <pch.h>

#include "TextElement.h"

namespace GuGu {
	TextElement::TextElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, const GuGuUtf8Str& text, uint32_t layer)
		: Element(elementType, geometry, layer, false)
		, m_color(color)
		, m_textInfo(textInfo)
		, m_text(text)
	{
	}
	TextElement::~TextElement()
	{
	}
}