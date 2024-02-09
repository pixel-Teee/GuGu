#pragma once

#include <Renderer/MyMath.h>

namespace GuGu {
	enum class HorizontalAlignment
	{
		Left,
		Center,
		Right,
		Stretch
	};

	enum class VerticalAlignment
	{
		Top,
		Center,
		Bottom,
		Stretch
	};

	typedef math::double4 Margin;
	typedef math::double4 Padding;
}