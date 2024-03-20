#include <pch.h>

#include "GuGuTextLayout.h"
#include "IRun.h"
#include "TextLayout.h"
#include "WidgetGeometry.h"
#include "ILayoutBlock.h"

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
		}

		return highestLayerId;
	}
}