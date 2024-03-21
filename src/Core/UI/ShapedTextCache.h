#pragma once

#include "TextRange.h"
#include "IRun.h"//shaped text context
#include "TextInfo.h"
#include <Core/HashCombine.h>

namespace GuGu {
	class ShapedTextCache;
	class CacheShapedTextKey;
	namespace ShapedTextCacheUtil
	{
		math::float2 measureShapedText(const std::shared_ptr<ShapedTextCache>& inShapedTextCache, const CacheShapedTextKey& inRunKey, const TextRange& inMeasureRange,
			const GuGuUtf8Str& inText);

		std::shared_ptr<ShapedGlyphSequence> getShapedTextSubSequence(const std::shared_ptr<ShapedTextCache>& inShapedTextCache,
			const CacheShapedTextKey& inRunKey, const TextRange& inTextRange, const GuGuUtf8Str& inText);
	}

	class CacheShapedTextKey //相比于FontKey，多了TextRange和ShapedTextContext，描述缓存一段文本范围的cache
	{
	public:
		CacheShapedTextKey(const TextRange& inTextRange, const float inScale, const ShapedTextContext& inTextContext, const TextInfo& inFontInfo)
			: m_textRange(inTextRange)
			, m_scale(inScale)
			, m_textContext(inTextContext)
			, m_fontInfo(inFontInfo)
		{

		}

		bool operator==(const CacheShapedTextKey& other) const
		{
			return m_textRange == other.m_textRange
				&& m_scale == other.m_scale
				&& m_textContext == other.m_textContext
				&& m_fontInfo == other.m_fontInfo;
		}

		TextRange m_textRange;
		float m_scale;
		ShapedTextContext m_textContext;
		TextInfo m_fontInfo;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::CacheShapedTextKey>
	{
		size_t operator()(const GuGu::CacheShapedTextKey& s) const
		{
			size_t keyHash = 0;
			GuGu::hash_combine(keyHash, s.m_textRange);
			GuGu::hash_combine(keyHash, s.m_scale);
			GuGu::hash_combine(keyHash, s.m_textContext);
			GuGu::hash_combine(keyHash, s.m_fontInfo);
			return keyHash;
		}
	};
}

namespace GuGu
{
	class FontCache;
	class ShapedTextCache
	{
	public:
		static std::shared_ptr<ShapedTextCache> Create(const std::shared_ptr<FontCache>& inFontCache);

		ShapedTextCache(const std::shared_ptr<FontCache>& inFontCache)
			: m_fontCachePtr(inFontCache)
		{}

		std::shared_ptr<ShapedGlyphSequence> findShapedText(const CacheShapedTextKey& inKey);

		std::shared_ptr<ShapedGlyphSequence> addShapedText(const CacheShapedTextKey& inKey, const GuGuUtf8Str& inText);

		std::shared_ptr<ShapedGlyphSequence> addShapedText(const CacheShapedTextKey& inKey, std::shared_ptr<ShapedGlyphSequence> inShapedText);

		std::shared_ptr<ShapedGlyphSequence> findOrAddShapedText(const CacheShapedTextKey& inKey, const GuGuUtf8Str& inText);
	private:
		std::weak_ptr<FontCache> m_fontCachePtr;

		std::unordered_map<CacheShapedTextKey, std::shared_ptr<ShapedGlyphSequence>> m_cachedShapedText;
	};
}

