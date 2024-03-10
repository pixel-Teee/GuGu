#pragma once

#include "Element.h"

namespace GuGu {
	class SplineElement : public Element
	{
	public:
		SplineElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer);

		virtual ~SplineElement();

		void setHermiteSpline(const math::float2& inStart, const math::float2& inStartDir, const math::float2& inEnd, const math::float2& inEndDir, float thickNess, const math::float4& inColor);

		float m_thickNess;

		math::float2 P0;
		math::float2 P1;
		math::float2 P2;
		math::float2 P3;

		math::float4 m_color;
	};
}