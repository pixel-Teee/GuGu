#include <pch.h>

#include "FreeTypeUtils.h"

namespace GuGu {
	namespace FreeTypeUtils
	{
		void ApplySizeAndScale(FT_Face inFace, const int32_t inFontSize, const float inFontScale)
		{
			const FT_F26Dot6 fixedFontSize = inFontSize << 6;
			FT_F26Dot6 requiredFixedFontPixelSize = 0;
			requiredFixedFontPixelSize = fixedFontSize;

			//convert the 26.6 scaled pixel size back into pixel space
			const uint32_t requiredFontPixelSize = ((requiredFixedFontPixelSize + (1 << 5)) >> 6);

			//set the pixel size
			FT_Set_Pixel_Sizes(inFace, requiredFontPixelSize, requiredFontPixelSize);
		}
	}
}