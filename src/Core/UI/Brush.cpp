#include <pch.h>

#include "Brush.h"
#include "StyleSetCenter.h"

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

		m_tintColorStr = "";
		m_tintColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	Brush::~Brush()
	{
	}

	math::float4 Brush::getTintColor()
	{
		if (m_tintColorStr != "")
		{
			std::shared_ptr<StyleSet> styleSetCenter = StyleSetCenter::GetStyle("EditorStyleSet");
			if (styleSetCenter)
			{
				return styleSetCenter->getColor(m_tintColorStr);
			}
			std::shared_ptr<StyleSet> coreSetCenter = StyleSetCenter::GetStyle("CoreStyleSet");
			if (coreSetCenter)
			{
				return coreSetCenter->getColor(m_tintColorStr);
			}
		}
		
		return m_tintColor;
	}

}