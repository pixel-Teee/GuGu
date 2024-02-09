#include <pch.h>

#include "BoxElement.h"

namespace GuGu {
	//BoxElement::BoxElement()
	//{
	//}
	BoxElement::BoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush)
		: Element(elementType, geometry)
		, m_color(color)
		, m_brush(brush)
	{
	}
	BoxElement::~BoxElement()
	{
	}
}