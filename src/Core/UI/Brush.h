#pragma once

#include <Renderer/nvrhi.h>

namespace GuGu {
	class Brush
	{
	public:
		Brush();

		virtual ~Brush();
	private:
		nvrhi::TextureHandle m_texture;
	};
}