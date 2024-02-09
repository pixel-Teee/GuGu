#pragma once

#include "Element.h"

namespace GuGu {
	class Brush;
	class BoxElement : public Element
	{
	public:
		BoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush);

		virtual ~BoxElement();

	private:
		math::float4 m_color;

		std::shared_ptr<Brush> m_brush;
	};
}