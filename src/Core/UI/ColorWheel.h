#pragma once

#include "Widget.h"
#include "Brush.h"
#include "UIMacros.h"
#include "Attribute.h"
#include "WidgetDelegates.h"

namespace GuGu {
	//色轮控件
	class ColorWheel : public Widget
	{
	public:
		ColorWheel();

		virtual ~ColorWheel();

		struct BuilderArguments : public Arguments<ColorWheel>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(Color, SelectedColor)

			UI_EVENT(SimpleDelegate, OnMouseCaptureBegin)

			UI_EVENT(SimpleDelegate, OnMouseCaptureEnd)

			UI_EVENT(OnLinearColorValueChanged, OnValueChanged)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
		virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
		virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;
	protected:
		math::float2 calcRelativePositionFromCenter() const;

		bool processMouseAction(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent, bool bProcessWhenOutsideColorWheel);
	private:
		//去展示的色轮图片
		std::shared_ptr<Brush> m_image;

		//被用户选择的现在的颜色，范围处于[0, 1]的颜色
		Attribute<Color> m_selectedColor;

		//用于展示的颜色选择器
		std::shared_ptr<Brush> m_selectorImage;

	private:
		//被调用，当鼠标被按下，并且一个捕获开始
		SimpleDelegate m_onMouseCaptureBegin;

		SimpleDelegate m_onMouseCaptureEnd;

		OnLinearColorValueChanged m_onValueChanged;
	};
}