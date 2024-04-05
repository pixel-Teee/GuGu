#pragma once

#include "Element.h"

namespace GuGu {
	class Brush;
	class BoxElement : public Element
	{
	public:
		BoxElement(ElementType elementType, WidgetGeometry geometry, math::float4 color, std::shared_ptr<Brush> brush, uint32_t layer, bool tiling);

		virtual ~BoxElement();

		friend class ElementList;
	protected:
		math::float4 m_color;

		std::shared_ptr<Brush> m_brush;
	};
}