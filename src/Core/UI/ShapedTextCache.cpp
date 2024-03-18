#include <pch.h>

#include "ShapedTextCache.h"

namespace GuGu {
	std::shared_ptr<ShapedTextCache> ShapedTextCache::Create(const std::shared_ptr<FontCache>& inFontCache)
	{
		return std::make_shared<ShapedTextCache>(inFontCache);
	}
}