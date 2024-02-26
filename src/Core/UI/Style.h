#pragma once

#include <Core/Math/MyMath.h>
#include "TextInfo.h"

namespace GuGu {
	class Style
	{
	public:
		Style();

		virtual ~Style();
	};
	
	class TextBlockStyle : public Style
	{
	public:
		TextBlockStyle(std::shared_ptr<TextInfo> textInfo, math::float4 textColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f), math::float2 shadowOffset = math::float2(0.0f, 0.0f), math::float4 shadowColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		virtual ~TextBlockStyle();

		std::shared_ptr<TextInfo> m_textInfo;
		math::float4 m_textColor;
		math::float2 m_shadowOffset;
		math::float4 m_shadowColor;
	};
}