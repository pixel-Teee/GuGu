#include <pch.h>

#include "SplineElement.h"

namespace GuGu {
	SplineElement::SplineElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer)
		: Element(elementType, geometry, layer, false)
		, m_thickNess(1.0f)
	{
	}
	SplineElement::~SplineElement()
	{
	}
	void SplineElement::setHermiteSpline(const math::float2& inStart, const math::float2& inStartDir, const math::float2& inEnd, const math::float2& inEndDir, float thickNess, const math::float4& inColor)
	{
		m_color = inColor;
		P0 = inStart;
		P1 = inStart + inStartDir / 3.0f;
		P2 = inEnd - inEndDir / 3.0f;
		P3 = inEnd;
		m_thickNess = thickNess;
		m_color = inColor;
	}
}