#pragma once

#include "WidgetGeometry.h"

namespace GuGu {
	//class ElementList;
	class Element
	{
	public:
		enum ElementType
		{
			Box,
			Text
		};

		Element();

		Element(ElementType elementType, WidgetGeometry geometry, uint32_t layer);

		virtual ~Element();

		friend class ElementList;

	private:
		ElementType m_elementType;

		WidgetGeometry m_geometry;

		uint32_t m_layer;
	};
}