#pragma once

#include "BoxElement.h"

namespace GuGu {
	class RoundedBoxElement : public BoxElement
	{
	public:
		RoundedBoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer, bool tiling);

		virtual ~RoundedBoxElement();

		void setRadius(math::float4 inRadius) { m_radius = inRadius; }
		math::float4 getRadius() const { return m_radius; }

		void setOutline(const math::float4& inOutlineColor, float inOutlineWeight) { m_outlineColor = inOutlineColor; m_outlineWeight = inOutlineWeight; }
		math::float4 getOutlineColor() const { return m_outlineColor; }
		float getOutlineWeight() const { return m_outlineWeight; }
	private:
		math::float4 m_outlineColor;

		math::float4 m_radius;

		//outline width
		float m_outlineWeight;
	};
}