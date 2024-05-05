#include <pch.h>

#include "GradientElement.h"

namespace GuGu {
	GradientElement::GradientElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer)
		: Element(elementType, geometry, layer, false)
	{
	}
	GradientElement::~GradientElement()
	{
	}
}