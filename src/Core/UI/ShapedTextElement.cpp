#include <pch.h>

#include "ShapedTextElement.h"
#include "ShapedTextCache.h"


namespace GuGu {
	ShapedTextElement::ShapedTextElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer)
		: Element(elementType, geometry, layer, false)
	{
	}
	ShapedTextElement::~ShapedTextElement()
	{
	}
}