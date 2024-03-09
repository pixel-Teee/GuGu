#pragma once

#include "Element.h"

namespace GuGu {
	class LineElement : public Element
	{
	public:
		LineElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling);

		virtual ~LineElement();
	
		float m_thickNess;

		std::vector<math::float2> m_points;

		math::float4 m_color;
	};
}