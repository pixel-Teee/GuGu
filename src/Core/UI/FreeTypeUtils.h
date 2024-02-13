#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace GuGu {
	namespace FreeTypeUtils
	{
		void ApplySizeAndScale(FT_Face inFace, const int32_t inFontSize, const float inFontScale);
	}
}