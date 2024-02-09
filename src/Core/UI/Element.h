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

		Element(ElementType elementType, WidgetGeometry geometry);

		virtual ~Element();

	private:
		ElementType m_elementType;

		WidgetGeometry m_geometry;
	};
}