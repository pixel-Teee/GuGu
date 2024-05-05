#pragma once

#include "WidgetGeometry.h"

namespace GuGu {
	struct GradientStop //渐变点
	{
		math::float2 m_position;
		math::float4 m_color;

		GradientStop(const math::float2& inPosition, const math::float4& inColor)
			: m_position(inPosition), m_color(inColor)
		{}
	};
	//class ElementList;
	class Element
	{
	public:
		enum ElementType
		{
			Box,
			Text,
			Line,
			Spline,
			Viewport,
			ShapedText,
			RoundedBox,
			Border,
			Gradient
		};

		Element();

		Element(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling);

		virtual ~Element();

		void setClipIndex(uint32_t clipIndex);

		friend class ElementList;

	private:
		ElementType m_elementType;

		WidgetGeometry m_geometry;

		uint32_t m_layer;

		bool m_tiling;

		uint32_t m_clipIndex;
	};
}