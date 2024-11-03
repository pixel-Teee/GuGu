#pragma once

#include "Widget.h"
#include "Brush.h"
#include "UIMacros.h"
#include "Attribute.h"
#include "WidgetDelegates.h"
#include "CoreStyle.h"
#include "Element.h"//GradientStop

namespace GuGu {
	enum class ColorBlockAlphaDisplayMode : uint8_t
	{
		//绘制一个单独的盒子，绘制颜色和透明度
		Combined,

		//盒子被分成两部分
		Separete,

		//忽视透明度
		Ignore
	};

	class ColorBlock : public Widget
	{
	public:
		ColorBlock();

		virtual ~ColorBlock();

		struct BuilderArguments : public Arguments<ColorBlock>
		{
			BuilderArguments() 
				: mColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
				, mAlphaBackgroundBrush(CoreStyle::getStyleSet()->getBrush("colorPicker.AlphaBckground"))
				, mcornerRadius(0.0f)
				, mcolorIsHsv(false)
				, mshowBackgroundForAlpha(false)
				, museSRGB(true)
				, malphaDisplayMode(ColorBlockAlphaDisplayMode::Combined)
				, mSize(math::float2(16.0f, 16.0f))
				, mOnMouseButtonDown()
			{}

			~BuilderArguments() = default;

			//用于显示的颜色
			ARGUMENT_ATTRIBUTE(math::float4, Color)

			//用于显示的背景颜色
			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, AlphaBackgroundBrush)

			//应用于 block 的 rounding
			ARGUMENT_ATTRIBUTE(math::float4, cornerRadius)

			ARGUMENT_ATTRIBUTE(bool, colorIsHsv)

			ARGUMENT_ATTRIBUTE(bool, showBackgroundForAlpha)

			ARGUMENT_ATTRIBUTE(bool, useSRGB)

			ARGUMENT_ATTRIBUTE(ColorBlockAlphaDisplayMode, alphaDisplayMode)

			ARGUMENT_ATTRIBUTE(math::float2, Size)

			UI_EVENT(PointerEventHandler, OnMouseButtonDown)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		void makeSection(std::vector<GradientStop>& outGradientStop, math::float2 startPt, math::float2 endPt, math::float4 color, bool bIgnoreAlpha) const;
	private:
		//用于显示这个颜色块的颜色
		Attribute<math::float4> m_color;

		std::shared_ptr<Brush> m_alphaBackgroundBrush;//透明度背景画刷

		Attribute<math::float4> m_gradientCornerRaidus;

		Attribute<math::float2> m_colorBlockSize;

		PointerEventHandler m_mouseButtonDownHandler;

		Attribute<ColorBlockAlphaDisplayMode> m_alphaDisplayMode;

		Attribute<bool> m_colorIsHsv;

		Attribute<bool> m_showBackgroundForAlpha;

		Attribute<bool> m_bUseSRGB;
	};
}