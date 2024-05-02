#pragma once

#include <Renderer/nvrhi.h>

#include <Core/Math/MyMath.h>

#include "EnumAsByte.h"
#include "BasicElement.h"

namespace GuGu {
	namespace BrushRoundingType
	{
		enum Type
		{
			//使用确切的半径
			FixedRadius,

			//rounding radius 应当是一半的高度，让它看起来很完美
			HalfHeightRadius
		};
	}
	struct BrushOutlineSettings
	{
		//应用在边缘每个角落的的半径，x = top left, y = top right, z = bottom right, w = bottom left
		math::float4 m_cornerRadius;

		//应用在border 边缘处的描边
		math::float4 m_color;

		//应用在border outline 的线条宽度
		float m_width;

		//round type
		EnumAsByte<BrushDrawType::Type> m_roundingType;

		BrushOutlineSettings(math::float4 inRadius, const math::float4& inColor, float inWidth)
			: m_cornerRadius(inRadius)
			, m_color(inColor)
			, m_width(inWidth)
			, m_roundingType(BrushDrawType::RoundedBox)
		{}
	};
	class Brush
	{
	public:
		Brush();

		virtual ~Brush();
	
		nvrhi::TextureHandle m_texture;//texture
		GuGuUtf8Str m_texturePath;
		math::double2 m_startUV;
		math::double2 m_sizeUV;
		math::int2 m_actualSize;
		Padding m_margin;//这个用于box 和 border 模式
		bool m_tiling;
		EnumAsByte<BrushDrawType::Type> m_drawAs;
		BrushOutlineSettings m_outlineSettings;
		math::float4 m_tintColor;
	};
}