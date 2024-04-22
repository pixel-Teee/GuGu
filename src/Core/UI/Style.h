#pragma once

#include <Core/Math/MyMath.h>

#include "TextInfo.h"
#include "BasicElement.h"

namespace GuGu {
	struct Style
	{
	public:
		Style();

		virtual ~Style();
	};
	
	class Brush;
	struct TextBlockStyle : public Style
	{
	public:
		TextBlockStyle(std::shared_ptr<TextInfo> textInfo, math::float4 textColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f), math::float2 shadowOffset = math::float2(0.0f, 0.0f), math::float4 shadowColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f));

		virtual ~TextBlockStyle();

		std::shared_ptr<TextInfo> m_textInfo;
		math::float4 m_textColor;
		math::float2 m_shadowOffset;
		math::float4 m_shadowColor;

		std::shared_ptr<Brush> m_highLightShape;
		TextBlockStyle& setHighLightShape(const std::shared_ptr<Brush> inHighLightShape) { m_highLightShape = inHighLightShape; return *this; }
	};

	struct ButtonStyle : public Style
	{
	public:
		ButtonStyle() = default;

		virtual ~ButtonStyle();

		std::shared_ptr<Brush> m_normal;
		ButtonStyle& setNormal(std::shared_ptr<Brush> inNormal) { m_normal = inNormal; return *this; }
		std::shared_ptr<Brush> m_hovered;
		ButtonStyle& setHovered(std::shared_ptr<Brush> inHovered) { m_hovered = inHovered; return *this; }
		std::shared_ptr<Brush> m_pressed;
		ButtonStyle& setPressed(std::shared_ptr<Brush> inPressed) { m_pressed = inPressed; return *this; }
		std::shared_ptr<Brush> m_disabled;
		ButtonStyle& setDisabled(std::shared_ptr<Brush> inDisabled) { m_disabled = inDisabled; return *this; }

		Padding m_pressedPadding;
		ButtonStyle& setPressedPadding(Padding inPressedPadding) { m_pressedPadding = inPressedPadding; return *this; }
	};

	struct CheckBoxStyle : public Style
	{
	public:
		CheckBoxStyle() = default;

		virtual ~CheckBoxStyle();

		std::shared_ptr<Brush> m_uncheckedImage;
		CheckBoxStyle& setUncheckedImage(std::shared_ptr<Brush> inUncheckedImage) { m_uncheckedImage = inUncheckedImage; return *this; }
		std::shared_ptr<Brush> m_checkedImage;
		CheckBoxStyle& setCheckedImage(std::shared_ptr<Brush> inCheckedImage) { m_checkedImage = inCheckedImage; return *this; }
		std::shared_ptr<Brush> m_backgroundImage;
		CheckBoxStyle& setBackgroundImage(std::shared_ptr<Brush> inBackgroundImage) { m_backgroundImage = inBackgroundImage; return *this; }

		Padding m_padding;
		CheckBoxStyle& setPadding(Padding inPadding) { m_padding = inPadding; return *this; }
	};

	struct SliderStyle : public Style
	{
	public:
		SliderStyle() = default;

		virtual ~SliderStyle();

		std::shared_ptr<Brush> m_normalBarImage;
		SliderStyle& setNormalBarImage(std::shared_ptr<Brush> inNormalBarImage) { m_normalBarImage = inNormalBarImage; return *this; }

		std::shared_ptr<Brush> m_normalThumbImage;
		SliderStyle& setNormalThumbImage(std::shared_ptr<Brush> inNormalThumbImage) { m_normalThumbImage = inNormalThumbImage; return *this; }

		float barThickNess;
		SliderStyle& setBarThickness(float inBarThickness) { barThickNess = inBarThickness; return *this; }
	};

	struct EditableTextBoxStyle : public Style
	{
	public:
		EditableTextBoxStyle() = default;

		virtual ~EditableTextBoxStyle();

		std::shared_ptr<Brush> m_backgroundImageNormal;
		EditableTextBoxStyle& setBackgroundImageNormal(std::shared_ptr<Brush> inNormalBackgroundImage) { m_backgroundImageNormal = inNormalBackgroundImage; return *this; }
	};

	struct SpinBoxStyle : public Style
	{
		SpinBoxStyle() = default;

		virtual ~SpinBoxStyle();

		std::shared_ptr<Brush> m_backgroundBrsuh;
		SpinBoxStyle& setBackgroundBrush(std::shared_ptr<Brush> inBackgroundBrush) { m_backgroundBrsuh = inBackgroundBrush; return *this; }
	};

	struct ScrollBarStyle : public Style
	{
		ScrollBarStyle() = default;

		virtual ~ScrollBarStyle();
	};
}