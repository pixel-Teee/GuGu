#pragma once

#include <Core/Math/MyMath.h>

#include "TextInfo.h"
#include "BasicElement.h"

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

	class Brush;
	class ButtonStyle : public Style
	{
		ButtonStyle();

		virtual ~ButtonStyle();

		std::shared_ptr<Brush> m_normal;
		ButtonStyle& setNormal(std::shared_ptr<Brush> inNormal) { m_normal = inNormal; }
		std::shared_ptr<Brush> m_hovered;
		ButtonStyle& setHovered(std::shared_ptr<Brush> inHovered) { m_hovered = inHovered; }
		std::shared_ptr<Brush> m_pressed;
		ButtonStyle& setPressed(std::shared_ptr<Brush> inPressed) { m_normal = inPressed; }
		std::shared_ptr<Brush> m_disabled;
		ButtonStyle& setDisabled(std::shared_ptr<Brush> inDisabled) { m_normal = inDisabled; }

		Padding m_pressedPadding;
		ButtonStyle& setPressedPadding(Padding inPressedPadding) { m_pressedPadding = inPressedPadding; }
	};
}