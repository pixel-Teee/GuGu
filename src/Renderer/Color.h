#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	struct Color
	{
		float r, g, b, a;

		Color() : r(0.f), g(0.f), b(0.f), a(0.f) { }
		Color(float c) : r(c), g(c), b(c), a(c) { }
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) { }
		Color(const math::float4& inColor) { r = inColor[0], g = inColor[1], b = inColor[2], a = inColor[3]; }

		math::float4 toFloat4() const { return math::float4(r, g, b, a); }

		bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
		bool operator !=(const Color& _b) const { return !(*this == _b); }
		float& component(int32_t index)
		{
			if (index == 0) return r;
			if (index == 1) return g;
			if (index == 2) return b;
			if (index == 3) return a;
		}

		Color HSVToLinearRGB() const;

		Color LinearRGBToHSV() const;

		GuGuUtf8Str toStr() const;
		void initFromStr(const GuGuUtf8Str& str);
	};
}