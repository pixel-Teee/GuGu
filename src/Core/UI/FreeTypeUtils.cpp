#include <pch.h>

#include "FreeTypeUtils.h"

namespace GuGu {
	namespace FreeTypeUtils
	{
		void ApplySizeAndScale(FT_Face inFace, const int32_t inFontSize, const float inFontScale)
		{
			const FT_F26Dot6 fixedFontSize = inFontSize << 6;
			const FT_Long fixedFontScale = inFontScale * 65536;
			FT_F26Dot6 requiredFixedFontPixelSize = 0;
			requiredFixedFontPixelSize = FT_MulFix(fixedFontSize, fixedFontScale);

			//convert the 26.6 scaled pixel size back into pixel space
			const uint32_t requiredFontPixelSize = ((requiredFixedFontPixelSize + (1 << 5)) >> 6);

			//set the pixel size
			FT_Set_Pixel_Sizes(inFace, requiredFontPixelSize, requiredFontPixelSize);
		}
		FT_Pos getDescender(FT_Face inFace)
		{
			return FT_MulFix(inFace->descender, inFace->size->metrics.y_scale);
		}
		FT_Pos getScaledHeight(FT_Face inFace)
		{
			return FT_MulFix(inFace->height, inFace->size->metrics.y_scale);
		}
	}
}