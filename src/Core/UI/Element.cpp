#include <pch.h>

#include "Element.h"

namespace GuGu {
	Element::Element()
		: m_elementType(ElementType::Box)
	{
	}
	Element::Element(ElementType elementType, WidgetGeometry geometry)
		: m_elementType(elementType)
		, m_geometry(geometry)
	{
	}
	Element::~Element()
	{
	}
}