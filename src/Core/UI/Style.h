#pragma once

#include <Core/Math/MyMath.h>

#include "TextInfo.h"
#include "BasicElement.h"
#include "EnumAsByte.h"

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
		ScrollBarStyle();

		virtual ~ScrollBarStyle();

		//当 scroll bar 方向水平的时候，使用的 background image
		std::shared_ptr<Brush> m_horizontalBackgroundImage;
		ScrollBarStyle& setHorizontalBackgroundImage(std::shared_ptr<Brush> inHorizontalBackgroundImage) { m_horizontalBackgroundImage = inHorizontalBackgroundImage; return *this; }

		//当 scroll bar 方向垂直的时候，使用的 background image
		std::shared_ptr<Brush> m_verticalBackgroundImage;
		ScrollBarStyle& setVerticalBackgroundImage(std::shared_ptr<Brush> inVerticalBackgroundImage) { m_verticalBackgroundImage = inVerticalBackgroundImage; return *this; }

		std::shared_ptr<Brush> m_verticalTopSlotImage;
		ScrollBarStyle& setVerticalTopSlotImage(std::shared_ptr<Brush> value) { m_verticalTopSlotImage = value; return *this; }

		std::shared_ptr<Brush> m_horizontalTopSlotImage;
		ScrollBarStyle& setHorizontalTopSlotImage(std::shared_ptr<Brush> value) { m_horizontalTopSlotImage = value; return *this; }

		std::shared_ptr<Brush> m_verticalBottomSlotImage;
		ScrollBarStyle& setVerticalBottomSlotImage(std::shared_ptr<Brush> value) { m_verticalBottomSlotImage = value; return *this; }

		std::shared_ptr<Brush> m_horizontalBottomSlotImage;
		ScrollBarStyle& setHorizontalBottomSlotImage(std::shared_ptr<Brush> value) { m_horizontalBottomSlotImage = value; return *this; }

		std::shared_ptr<Brush> m_normalThumbImage;
		ScrollBarStyle& setNormalThumbImage(std::shared_ptr<Brush> inNormalThumbImage) { m_normalThumbImage = inNormalThumbImage; return *this; }

		float m_thickNess;
		ScrollBarStyle& setThickness(float inThickness) { m_thickNess = inThickness; return *this; }
	};

	struct ComboButtonStyle : public Style
	{
		ComboButtonStyle();

		virtual ~ComboButtonStyle() {}

		std::shared_ptr<ButtonStyle> m_buttonStyle;
		ComboButtonStyle& setButtonStyle(const std::shared_ptr<ButtonStyle>& inButtonStyle) { m_buttonStyle = inButtonStyle; return *this; }

		//下箭头
		std::shared_ptr<Brush> m_downArrowImage;
		ComboButtonStyle& setDownArrowImage(const std::shared_ptr<Brush>& inDownArrowImage) { m_downArrowImage = inDownArrowImage; return *this; }

		//shadow 应当偏移 down arrow 多少？
		//0表示没有阴影
		math::float2 m_shadowOffset;
		ComboButtonStyle& setShadowOffset(const math::float2& inShadowOffset) { m_shadowOffset = inShadowOffset; return *this; }

		//下箭头的颜色和透明度
		math::float4 m_shadowColorAndOpacity;
		ComboButtonStyle& setShadowColorAndOpacity(const math::float4& inShadowColorAndOpacity) { m_shadowColorAndOpacity = inShadowColorAndOpacity; return *this; }

		//去添加一个 menu border 在 drop-down 内容
		std::shared_ptr<Brush> m_menuBorderBrush;
		ComboButtonStyle& setMenuBorderBrush(const std::shared_ptr<Brush>& inMenuBorderBrush) { m_menuBorderBrush = inMenuBorderBrush; return *this; }

		//添加一个 menu border padding 在 drop-down 内容周围
		Padding m_menuBorderPadding;
		ComboButtonStyle& setMenuBorderPadding(const Padding& inMenuBorderPadding) { m_menuBorderPadding = inMenuBorderPadding; return *this; }

		Padding m_contentPadding;
		ComboButtonStyle& setContentPadding(const Padding& inContentPadding) { m_contentPadding = inContentPadding; return *this; }

		//下箭头 padding
		Padding m_downArrowPadding;
		ComboButtonStyle& setDownArrowPadding(const Padding& inDownArrowPadding) { m_downArrowPadding = inDownArrowPadding; return *this; }

		//下箭头垂直对齐
		EnumAsByte<VerticalAlignment> m_downArrowAlign;
		ComboButtonStyle& setDownArrowAlignment(const VerticalAlignment& inValign) { m_downArrowAlign = inValign; return *this; }
	};

	struct TableRowStyle : public Style
	{
		TableRowStyle();

		virtual ~TableRowStyle();
	};
	
	struct ComboBoxStyle : public Style
	{
		ComboBoxStyle();

		virtual ~ComboBoxStyle();

		std::shared_ptr<ComboButtonStyle> m_comboButtonStyle;
		ComboBoxStyle& setComboButtonStyle(const std::shared_ptr<ComboButtonStyle> inComboButtonStyle) { m_comboButtonStyle = inComboButtonStyle; return *this; }

		//按钮的内容 padding
		Padding m_contentPadding;
		ComboBoxStyle& setContentPadding(const Padding& inContentPadding) { m_contentPadding = inContentPadding; return *this; }

		//菜单 row padding
		Padding m_menuRowPadding;
		ComboBoxStyle& setMenuRowPadding(const Padding& inMenuRowPadding) { m_menuRowPadding = inMenuRowPadding; return *this; }
	};
}