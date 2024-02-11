#include <pch.h>

#include "BoxElement.h"

namespace GuGu {
	//BoxElement::BoxElement()
	//{
	//}
	BoxElement::BoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer, bool tiling)
		: Element(elementType, geometry, layer, tiling)
		, m_color(color)
		, m_brush(brush)
	{
	}
	BoxElement::~BoxElement()
	{
	}
}