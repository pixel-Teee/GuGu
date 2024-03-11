#pragma once

#include "Element.h"
#include <Renderer/nvrhi.h>

namespace GuGu {
	class ViewportElement : public Element
	{
	public:
		ViewportElement(ElementType elementType, WidgetGeometry geometry, uint32_t layer, bool tiling);

		virtual ~ViewportElement();

		nvrhi::TextureHandle m_renderTargetResouce;

		math::float4 m_color;
	};
}