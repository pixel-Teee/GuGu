#pragma once

#include "Element.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class TextInfo;
	class GuGuUtf8Str;
	class TextElement : public Element
	{
	public:
		TextElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<TextInfo> textInfo, GuGuUtf8Str& text,uint32_t layer);

		virtual ~TextElement();
	
		math::float4 m_color;

		GuGuUtf8Str m_text;

		std::shared_ptr<TextInfo> m_textInfo;
	};
}