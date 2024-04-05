#include <pch.h>

#include "Brush.h"

namespace GuGu {
	Brush::Brush()
		//corner radius
		//color
		//width
		: m_outlineSettings(BrushOutlineSettings(math::float4(1.0f, 1.0f, 1.0f, 1.0f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f))
	{
		m_drawAs = BrushDrawType::Type::Image;

		m_margin = Padding(0, 0, 0, 0);
		//m_outlineSettings = BrushOutlineSettings(math::float4(1.0f, 1.0f, 1.0f, 1.0f), math::float4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
	}
	Brush::~Brush()
	{
	}
}