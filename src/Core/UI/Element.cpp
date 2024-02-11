#include <pch.h>

#include "Element.h"

namespace GuGu {
	Element::Element()
		: m_elementType(ElementType::Box)
		, m_layer(0)
	{
	}
	Element::Element(ElementType elementType, WidgetGeometry geometry, uint32_t layer)
		: m_elementType(elementType)
		, m_geometry(geometry)
		, m_layer(layer)
	{
	}
	Element::~Element()
	{
	}
}