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
	TextRun::~TextRun()
	{
	}
	TextRange TextRun::getTextRange() const
	{
		return m_range;
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
}