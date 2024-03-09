#include <pch.h>

#include "LineElement.h"

namespace GuGu {
	LineElement::LineElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling)
		: Element(elementType, geometry, layer, tiling)
		, m_thickNess(1.0f)
	{
	}
	LineElement::~LineElement()
	{
	}
}