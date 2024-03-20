#include <pch.h>

#include "ShapedTextCache.h"

namespace GuGu {
	namespace ShapedTextCacheUtil
	{
		math::float2 measureShapedText(const std::shared_ptr<ShapedTextCache>& inShapedTextCache, const CacheShapedTextKey& inRunKey, const TextRange& inMeasureRange,
			const GuGuUtf8Str& inText)
		{
			//测量塑形文本
			std::shared_ptr<ShapedGlyphSequence> shapedText = inShapedTextCache->findOrAddShapedText(inRunKey, inText);

			std::optional<int32_t> measuredWidth = shapedText->getMeasuredWidth(inMeasureRange.m_beginIndex, inMeasureRange.m_endIndex);


			return math::float2(measuredWidth.value(), shapedText->getMaxTextHeight());
		}
	}
	std::shared_ptr<ShapedTextCache> ShapedTextCache::Create(const std::shared_ptr<FontCache>& inFontCache)
	{
		return std::make_shared<ShapedTextCache>(inFontCache);
	}
	std::shared_ptr<ShapedGlyphSequence> ShapedTextCache::findShapedText(const CacheShapedTextKey& inKey)
	{
		//std::shared_ptr<ShapedGlyphSequence> shapedText = (*m_cachedShapedText.find(inKey)).second;
		//
		//return shapedText;

		if (m_cachedShapedText.find(inKey) != m_cachedShapedText.end())
			return (*m_cachedShapedText.find(inKey)).second;
		return nullptr;
	}
	std::shared_ptr<ShapedGlyphSequence> ShapedTextCache::addShapedText(const CacheShapedTextKey& inKey, const GuGuUtf8Str& inText)
	{
		const std::shared_ptr<FontCache> fonCache = m_fontCachePtr.lock();

		std::shared_ptr<ShapedGlyphSequence> shapedText = fonCache->shapeUnidirectionalText(
			inText,
			inKey.m_textRange.m_beginIndex,
			inKey.m_textRange.len(),
			inKey.m_fontInfo,
			inKey.m_scale,
			inKey.m_textContext.m_textShapingMethod
		);

		return addShapedText(inKey, shapedText);
	}
	std::shared_ptr<ShapedGlyphSequence> ShapedTextCache::addShapedText(const CacheShapedTextKey& inKey, std::shared_ptr<ShapedGlyphSequence> inShapedText)
	{
		m_cachedShapedText.insert({ inKey, inShapedText });
		return inShapedText;
	}
	std::shared_ptr<ShapedGlyphSequence> ShapedTextCache::findOrAddShapedText(const CacheShapedTextKey& inKey, const GuGuUtf8Str& inText)
	{
		std::shared_ptr<ShapedGlyphSequence> shapedText = findShapedText(inKey);

		if (!shapedText)
		{
			shapedText = addShapedText(inKey, inText);
		}

		return shapedText;
	}
}