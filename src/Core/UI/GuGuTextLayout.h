#pragma once

#include "TextLayout.h"
#include "Style.h"

namespace GuGu {
	class Widget;
	class GuGuTextLayout : public TextLayout
	{
	public:
		GuGuTextLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle);

		virtual ~GuGuTextLayout();

		const TextBlockStyle& getDefaultTextStyle() const;

		int32_t OnPaint(const PaintArgs& inPaintArgs, const WidgetGeometry& inAllottedGeometry, const math::box2& cullingRect, ElementList& elementList, int32_t layerId, const Style& inWidgetStyle);

		virtual int32_t OnPaintHighlights(const PaintArgs& args, const TextLayout::LineView& lineView, const std::vector<LineViewHighlight>& hightlights, const TextBlockStyle& defaultTextStyle, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect, ElementList& elementList, int32_t layerId) const;
	private:
		TextBlockStyle m_defaultTextStyle;

		//todo:handle this
		//std::shared_ptr<Widget> m_children;
	};
}