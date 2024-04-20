#include <pch.h>

#include "TextEditHelper.h"
#include "FontCache.h"

namespace GuGu {
	float TextEditHelper::getFontHeight(const TextInfo& fontInfo)
	{
		const std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		return fontCache->getMaxCharacterHeight(fontInfo, 1.0f);
	}
	float TextEditHelper::calculateCaretWidth(const float fontMaxCharHeight)
	{
		return std::clamp(0.08f * fontMaxCharHeight, 1.0f, 2.0f);
	}
}