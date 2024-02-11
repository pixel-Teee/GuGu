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

	struct UIVertex
	{
		math::float4 textureCoordinate;
		math::float2 position;
		math::float4 color;
		math::float4 secondaryColor;
		UIVertex(math::float4 inTextureCoordinate, math::float2 inPosition, math::float4 inColor, math::float4 inSecondaryColor)
			: textureCoordinate(inTextureCoordinate), position(inPosition), color(inColor), secondaryColor(inSecondaryColor)
		{}
	};
}