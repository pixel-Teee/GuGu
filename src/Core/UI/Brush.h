#pragma once

#include <Renderer/nvrhi.h>

#include <Core/Math/MyMath.h>

namespace GuGu {
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
		bool m_tiling;
	};
}