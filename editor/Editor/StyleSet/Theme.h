#pragma once

#include <Renderer/nvrhi.h>

namespace GuGu {
	enum ThemeKeys
	{
		ColorLevel1,
		ColorLevel2,
		ColorLevel3,
		ColorLevel4,
		ColorLevel5,
		ColorLevel6,
		ColorLevel7,
		LightColorLevel1,
		LightColorLevel2,
		SecondaryColorLevel1,
		SecondaryColorLevel2,
		SecondaryColorLevel3,
		SecondaryColorLevel4,
		SecondaryColorLevel5,
		SecondaryColorLevel6,
		SecondaryColorLevel7,
		SecondaryColorLevel8,
		SecondaryColorLevel9,
		GrayColor,
		GrayHighLightColor,
		WhiteColor,
		MaxColorCount
	};
	
	struct Theme//one theme
	{
		//index is enum ThemeKeys
		Array<math::float4> m_colors;

		Theme() 
		{
			m_colors.resize(ThemeKeys::MaxColorCount);
		}

		static GuGuUtf8Str ThemeKeysToStr(ThemeKeys key);

		static ThemeKeys StrToThemeKeys(const GuGuUtf8Str& str);
	};
}