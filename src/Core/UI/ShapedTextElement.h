#pragma once

#include "Element.h"

namespace GuGu {
	class ShapedGlyphSequence;
	class ShapedTextElement : public Element
	{
	public: 
		ShapedTextElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer);

		virtual ~ShapedTextElement();

		std::shared_ptr<ShapedGlyphSequence> m_shapedTextSequence;

		math::float4 m_color;
	};
}