#pragma once

#include "Element.h"
#include "BasicElement.h"//Orientation

namespace GuGu {
	class GradientElement : public Element
	{
	public:
		GradientElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer);

		virtual ~GradientElement();

		Orientation m_orientation;

		std::vector<GradientStop> m_gradientStops;

		math::float4 m_cornerRadius;
	};
}