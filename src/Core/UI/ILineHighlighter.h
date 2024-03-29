#pragma once

#include "TextLayout.h"

namespace GuGu {
	struct PaintArgs;
	class TextBlockStyle;
	class ILineHighlighter
	{
	public:

		virtual ~ILineHighlighter() {}

		virtual int32_t onPaint(const PaintArgs& args, const TextLayout::LineView& line, const float offsetX, const float inWidth, const TextBlockStyle& defaultStyle, const WidgetGeometry& allottedGeometry,
			const math::box2& cullingRect, ElementList& outDrawElements, int32_t layerId) const = 0;
	};
}