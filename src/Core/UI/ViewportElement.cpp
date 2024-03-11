#include <pch.h>

#include "ViewportElement.h"

namespace GuGu {
	ViewportElement::ViewportElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling)
		: Element(elementType, geometry, layer, tiling)
	{
	}
	ViewportElement::~ViewportElement()
	{
	}
}