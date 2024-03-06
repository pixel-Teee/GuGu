#pragma once

namespace GuGu {
	struct Color
	{
		float r, g, b, a;

		Color() : r(0.f), g(0.f), b(0.f), a(0.f) { }
		Color(float c) : r(c), g(c), b(c), a(c) { }
		Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) { }

		bool operator ==(const Color& _b) const { return r == _b.r && g == _b.g && b == _b.b && a == _b.a; }
		bool operator !=(const Color& _b) const { return !(*this == _b); }
	};
}