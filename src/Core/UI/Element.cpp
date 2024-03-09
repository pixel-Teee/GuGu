#include <pch.h>

#include "Element.h"

namespace GuGu {
	Element::Element()
		: m_elementType(ElementType::Box)
		, m_layer(0)
		, m_tiling(false)
		, m_clipIndex(-1)
	{
	}
	Element::Element(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling)
		: m_elementType(elementType)
		, m_geometry(geometry)
		, m_layer(layer)
		, m_tiling(tiling)
		, m_clipIndex(-1)
	{
	}
	Element::~Element()
	{
	}
	void Element::setClipIndex(uint32_t clipIndex)
	{
		m_clipIndex = clipIndex;
	}
}