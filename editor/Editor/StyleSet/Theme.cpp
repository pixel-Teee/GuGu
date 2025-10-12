#include <pch.h>

#include "Theme.h"

namespace GuGu {

	GuGuUtf8Str Theme::ThemeKeysToStr(ThemeKeys key)
	{
		switch (key)
		{
			case ColorLevel1		 : return "ColorLevel1";
			case ColorLevel2		 : return "ColorLevel2";
			case ColorLevel3		 : return "ColorLevel3";
			case ColorLevel4		 : return "ColorLevel4";
			case ColorLevel5		 : return "ColorLevel5";
			case ColorLevel6		 : return "ColorLevel6";
			case ColorLevel7		 : return "ColorLevel7";
			case LightColorLevel1	 : return "LightColorLevel1";
			case LightColorLevel2	 : return "LightColorLevel2";
			case SecondaryColorLevel1: return "SecondaryColorLevel1";
			case SecondaryColorLevel2: return "SecondaryColorLevel2";
			case SecondaryColorLevel3: return "SecondaryColorLevel3";
			case SecondaryColorLevel4: return "SecondaryColorLevel4";
			case SecondaryColorLevel5: return "SecondaryColorLevel5";
			case SecondaryColorLevel6: return "SecondaryColorLevel6";
			case SecondaryColorLevel7: return "SecondaryColorLevel7";
			case SecondaryColorLevel8: return "SecondaryColorLevel8";
			case SecondaryColorLevel9: return "SecondaryColorLevel9";
			case GrayColor			 : return "GrayColor";
			case GrayHighLightColor  : return "GrayHighLightColor";
			case WhiteColor			 : return "WhiteColor";
		}
		return "";//nothing
	}

	ThemeKeys Theme::StrToThemeKeys(const GuGuUtf8Str& str)
	{
		if (str == "ColorLevel1")
			return ColorLevel1;
		else if (str == "ColorLevel2")
			return ColorLevel2;
		else if (str == "ColorLevel3")
			return ColorLevel3;
		else if (str == "ColorLevel4")
			return ColorLevel4;
		else if (str == "ColorLevel5")
			return ColorLevel5;
		else if (str == "ColorLevel6")
			return ColorLevel6;
		else if (str == "ColorLevel7")
			return ColorLevel7;
		else if (str == "LightColorLevel1")
			return LightColorLevel1;
		else if (str == "LightColorLevel2")
			return LightColorLevel2;
		else if (str == "SecondaryColorLevel1")
			return SecondaryColorLevel1;
		else if (str == "SecondaryColorLevel2")
			return SecondaryColorLevel2;
		else if (str == "SecondaryColorLevel3")
			return SecondaryColorLevel3;
		else if (str == "SecondaryColorLevel4")
			return SecondaryColorLevel4;
		else if (str == "SecondaryColorLevel5")
			return SecondaryColorLevel5;
		else if (str == "SecondaryColorLevel6")
			return SecondaryColorLevel6;
		else if (str == "SecondaryColorLevel7")
			return SecondaryColorLevel7;
		else if (str == "SecondaryColorLevel8")
			return SecondaryColorLevel8;
		else if (str == "SecondaryColorLevel9")
			return SecondaryColorLevel9;
		else if (str == "GrayColor")
			return GrayColor;
		else if (str == "GrayHighLightColor")
			return GrayHighLightColor;
		else if (str == "WhiteColor")
			return WhiteColor;
	}

}