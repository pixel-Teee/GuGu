#include <pch.h>

#include "RoundedBoxElement.h"

namespace GuGu {
	RoundedBoxElement::RoundedBoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer, bool tiling)
		: BoxElement(elementType, geometry, color, brush, layer, tiling)
	{
	}
	RoundedBoxElement::~RoundedBoxElement()
	{
	}
}