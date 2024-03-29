#include <pch.h>

#include "GuGuTextLayout.h"
#include "IRun.h"
#include "TextLayout.h"
#include "WidgetGeometry.h"
#include "ILayoutBlock.h"
#include "ILineHighlighter.h"
#include "TextRun.h"

namespace GuGu {
	GuGuTextLayout::GuGuTextLayout(Widget* inOwner, TextBlockStyle inDefaultTextStyle)
		: m_defaultTextStyle(inDefaultTextStyle)
	{
	}
	GuGuTextLayout::~GuGuTextLayout()
	{
	}
	const TextBlockStyle& GuGuTextLayout::getDefaultTextStyle() const
	{
		return m_defaultTextStyle;
	}
	int32_t GuGuTextLayout::OnPaint(const PaintArgs& inPaintArgs, const WidgetGeometry& inAllottedGeometry, const math::box2& cullingRect, ElementList& elementList, int32_t layerId, const Style& inWidgetStyle)
	{
		const float inverseScale = 1.0f / inAllottedGeometry.mAbsoluteScale;

		int32_t highestLayerId = layerId;

		for (int32_t lineIndex = 0; lineIndex < m_lineViews.size(); ++lineIndex)
		{
			const TextLayout::LineView& lineView = m_lineViews[lineIndex];

			//渲染这行的每个块
			for (const std::shared_ptr<ILayoutBlock>& block : lineView.blocks)
			{
				TextArgs textArgs(lineView, block, m_defaultTextStyle);
				
				const std::shared_ptr<IRun> run = block->getRun();

				highestLayerId = run->OnPaint(inPaintArgs, textArgs, inAllottedGeometry, cullingRect, elementList, layerId, inWidgetStyle);
			}

			const int32_t highestOverlayId = OnPaintHighlights(inPaintArgs, lineView, lineView.m_overlayHighlights, m_defaultTextStyle, inAllottedGeometry, cullingRect, elementList, highestLayerId);
			highestLayerId = std::max(highestLayerId, highestOverlayId);
		}

		return highestLayerId;
	}
	int32_t GuGuTextLayout::OnPaintHighlights(const PaintArgs& args, const TextLayout::LineView& lineView, const std::vector<LineViewHighlight>& hightlights, const TextBlockStyle& defaultTextStyle, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect, ElementList& elementList, int32_t layerId) const
	{
		int32_t currentLayerId = layerId;

		for (const LineViewHighlight& highLight : hightlights)
		{
			const std::shared_ptr<ILineHighlighter> lineHighlighter = std::static_pointer_cast<ILineHighlighter>(highLight.m_highLighter);

			if (lineHighlighter)
			{
				currentLayerId = lineHighlighter->onPaint(args, lineView, highLight.m_offsetX, highLight.m_width, defaultTextStyle, allottedGeometry, cullingRect, elementList, currentLayerId);
			}
		}

		return currentLayerId;
	}
	std::shared_ptr<IRun> GuGuTextLayout::createDefaultTextRun(const std::shared_ptr<GuGuUtf8Str>& newText, const TextRange& newRange) const
	{
		return std::make_shared<TextRun>(RunInfo(), newText, m_defaultTextStyle, newRange);
	}
}