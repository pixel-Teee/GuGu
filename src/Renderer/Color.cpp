#include <pch.h>

#include "Color.h"

namespace GuGu {

	Color Color::HSVToLinearRGB() const
	{
		const float hue = r;
		const float saturation = g;
		const float value = b;

		const float hdiv60 = hue / 60.0f;
		const float hdiv60_floor = std::floorf(hdiv60);
		const float hdiv60_fraction = hdiv60 - hdiv60_floor;

		const float rgbValues[4] = {
			value,
			value * (1.0f - saturation),
			value * (1.0f - (hdiv60_fraction * saturation)),
			value * (1.0f - ((1.0f - hdiv60_fraction) * saturation))
		};

		const uint32_t rgbSwizzle[6][3] = {
			{0, 3, 1},
			{2, 0, 1},
			{1, 0, 3},
			{1, 2, 0},
			{3, 1, 0},
			{0, 1, 2}
		};

		const uint32_t swizzleIndex = ((uint32_t)hdiv60_floor) % 6;

		return Color(rgbValues[rgbSwizzle[swizzleIndex][0]],
					 rgbValues[rgbSwizzle[swizzleIndex][1]],
					 rgbValues[rgbSwizzle[swizzleIndex][2]],
					 a);
	}

	Color Color::LinearRGBToHSV() const
	{
		const float rgbMin = std::min(std::min(r, g), b);
		const float rgbMax = std::max(std::max(r, g), b);
		const float rgbRange = rgbMax - rgbMin;

		const float hue = (rgbMax == rgbMin ? 0.0f :
						   rgbMax == r ? std::fmod((((g - b) / rgbRange) * 60.0f) + 360.0f, 360.0f) :
						   rgbMax == g ?		   (((b - r) / rgbRange) * 60.0f) + 120.0f :
						   rgbMax == b ?           (((r - g) / rgbRange) * 60.0f) + 240.0f :
						   0.0f);
		const float saturation = (rgbMax == 0.0f ? 0.0f : rgbRange / rgbMax);
		const float value = rgbMax;

		return Color(hue, saturation, value, a);
	}

	GuGuUtf8Str Color::toStr() const
	{
		char buf[64];
		std::sprintf(buf, "(r = %f, g = %f, b = %f, a = %f)", r, g, b, a);
		return buf;
	}

	void Color::initFromStr(const GuGuUtf8Str& str)
	{
		std::sscanf(str.getStr(), "(r = %f, g = %f, b = %f, a = %f)", &r, &g, &b, &a);
	}

}