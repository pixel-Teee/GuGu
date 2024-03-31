#include <pch.h>

#include "TextRun.h"
#include "DefaultLayoutBlock.h"
#include "IRun.h"
#include "ShapedTextCache.h"
#include "WidgetGeometry.h"
#include "ElementList.h"

#include <Core/Math/MyMath.h>

namespace GuGu {
	TextRun::TextRun(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style)
		: m_runInfo(inRunInfo)
		, m_text(inText)
		, m_style(style)
		, m_range(0, inText->len())
	{
	}
	TextRun::TextRun(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style, const TextRange& inRange)
		: m_runInfo(inRunInfo)
		, m_text(inText)
		, m_style(style)
		, m_range(inRange)
	{
	}
	TextRun::~TextRun()
	{
	}
	TextRange TextRun::getTextRange() const
	{
		return m_range;
	}
	void TextRun::setTextRange(const TextRange& value)
	{
		m_range = value;
	}
	
	int16_t TextRun::getMaxHeight(float scale) const
	{
		const std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		return fontCache->getMaxCharacterHeight(*m_style.m_textInfo, scale);
	}
	int16_t TextRun::getBaseLine(float inScale) const
	{
		const std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		return fontCache->getBaseLine(*m_style.m_textInfo, inScale);
	}
	std::shared_ptr<TextRun> TextRun::Create(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style)
	{
		return std::make_shared<TextRun>(inRunInfo, inText, style);
	}
	std::shared_ptr<TextRun> TextRun::Create(const RunInfo& inRunInfo, const std::shared_ptr<const GuGuUtf8Str>& inText, const TextBlockStyle& style, const TextRange& inRange)
	{
		return std::make_shared<TextRun>(inRunInfo, inText, style, inRange);
	}
	std::shared_ptr<ILayoutBlock> TextRun::createBlock(int32_t startIndex, int32_t endIndex, math::float2 size, const LayoutBlockTextContext& textContext)
	{
		return DefaultLayoutBlock::Create(shared_from_this(), TextRange(startIndex, endIndex), size, textContext);
	}
	math::float2 TextRun::measure(int32_t startIndex, int32_t endIndex, float scale, const RunTextContext& textContext) const
	{
		//todo:添加阴影offset和边框大小
		if (endIndex - startIndex == 0)
		{
			return math::float2(0, getMaxHeight(scale));
		}

		//测量塑形文本
		return ShapedTextCacheUtil::measureShapedText(textContext.m_shapedTextCache, CacheShapedTextKey(TextRange(0, m_text->len()), scale, textContext, *m_style.m_textInfo), TextRange(startIndex, endIndex), *m_text);
	}
	int32_t TextRun::OnPaint(const PaintArgs& paintArgs, const TextArgs& textArgs, const WidgetGeometry& allottedGeometry, const math::box2& cullingRect, ElementList& outDrawElements, int32_t layerId, const Style& inWidgetStyle) const
	{
		const std::shared_ptr<ILayoutBlock>& Block = textArgs.block;
		const TextBlockStyle& DefaultStyle = textArgs.defaultStyle;
		const TextLayout::LineView& Line = textArgs.line;
		const TextRange blockRange = Block->getTextRange();
		const LayoutBlockTextContext BlockTextContext = Block->getTextContext();

		const float inverseScale = 1.0f / allottedGeometry.mAbsoluteScale;

		std::shared_ptr<ShapedGlyphSequence> shapedText = ShapedTextCacheUtil::getShapedTextSubSequence(
			BlockTextContext.m_shapedTextCache,
			CacheShapedTextKey(Line.range, allottedGeometry.getAccumulateTransform().m_linear[0][0], BlockTextContext, *m_style.m_textInfo),//todo:这里传递了一个缩放，暂时直接取，以后增加getScale函数
			blockRange,
			*m_text
		);

		math::affine2 inverseScaleTransform = math::affine2(math::float2(inverseScale, 0), math::float2(0, inverseScale), math::float2(0, 0));

		ElementList::addShapedTextElement(outDrawElements,
			allottedGeometry.getChildGeometry(inverseScaleTransform.transformVector(Block->getSize()), inverseScaleTransform.transformPoint(Block->getLocationOffset()), allottedGeometry.getAccumulateTransform()),
			math::float4(1.0f, 1.0f, 1.0f, 1.0f),
			shapedText,
			layerId + 1
		);

		return layerId + 1;
	}
	void TextRun::move(const std::shared_ptr<GuGuUtf8Str>& newText, const TextRange& newRange)
	{
		m_text = newText;
		m_range = newRange;
	}
	std::shared_ptr<IRun> TextRun::clone()
	{
		return TextRun::Create(m_runInfo, m_text, m_style, m_range);
	}
	void TextRun::appendTextTo(GuGuUtf8Str& text) const
	{
		GuGuUtf8Str subStr = m_text->substr(m_range.m_beginIndex, m_range.len());
		text.append(subStr);
	}
	void TextRun::appendTextTo(GuGuUtf8Str& text, const TextRange& range) const
	{
		GuGuUtf8Str subStr = m_text->substr(range.m_beginIndex, range.len());
		text.append(subStr);
	}
	int32_t TextRun::getTextIndexAt(const std::shared_ptr<ILayoutBlock>& block, const math::float2& location, float scale) const
	{
		const math::float2& blockOffset = block->getLocationOffset();
		const math::float2& blockSize = block->getSize();

		const float left = blockOffset.x;
		const float top = blockOffset.y;
		const float right = blockOffset.x + blockSize.x;
		const float bottom = blockOffset.y + blockSize.y;

		const bool containsPoint = location.x >= left && location.x < right && location.y >= top && location.y < bottom;

		if (!containsPoint)
		{
			return -1;
		}

		const TextRange BlockRange = block->getTextRange();
		const LayoutBlockTextContext BlockTextContext = block->getTextContext();

		const int32_t index = ShapedTextCacheUtil::findCharacterIndexAtOffset(BlockTextContext.m_shapedTextCache, CacheShapedTextKey(TextRange(0, m_text->len()), scale, BlockTextContext, *m_style.m_textInfo),
			BlockRange, *m_text, location.x - blockOffset.x);

		return index;
	}
}