#pragma once

#include "IRun.h"

#include <memory>

#include "Style.h"//TextBlockStyle
#include "TextRange.h"

namespace GuGu {
	//class TextBlockStyle;
	class TextRun : public IRun, public std::enable_shared_from_this<TextRun>
	{
	public:
		TextRun(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style);

		virtual ~TextRun();

		virtual TextRange getTextRange() const override;

		virtual void setTextRange(const TextRange& value) override;

		virtual int16_t getMaxHeight(float scale) const override;

		virtual int16_t getBaseLine(float inScale) const override;

		static std::shared_ptr<TextRun> Create(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style);

		virtual std::shared_ptr<ILayoutBlock> createBlock(int32_t startIndex, int32_t endIndex, math::float2 size, const LayoutBlockTextContext& textContext) override;

		virtual math::float2 measure(int32_t startIndex, int32_t endIndex, float scale, const RunTextContext& textContext) const override;

		virtual int32_t OnPaint(const PaintArgs& paintArgs, const TextArgs& textArgs, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect,
			ElementList& outDrawElements, int32_t layerId, const Style& inWidgetStyle) const override;

		virtual void appendTextTo(GuGuUtf8Str& text) const override;

		virtual int32_t getTextIndexAt(const std::shared_ptr<ILayoutBlock>& block, const math::float2& location, float scale) const override;
	protected:
		RunInfo m_runInfo;
		std::shared_ptr<const GuGuUtf8Str> m_text;
		TextBlockStyle m_style;
		TextRange m_range;
	};
}