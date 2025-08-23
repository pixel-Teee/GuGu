#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "BasicElement.h"
#include "WidgetDelegates.h"

namespace GuGu {
	class Brush;
	struct SliderStyle;

	class Slider : public Widget
	{
	public:
		Slider();

		virtual ~Slider();

		struct BuilderArguments : public Arguments<Slider>
		{
			BuilderArguments()	
				: morientation(Orientation::Horizontal)
				, mIsFocusable(true)
				, msliderBarColor(Color(1.0f, 1.0f, 1.0f, 1.0f))//white
				, mMinValue(0.0f)
				, mMaxValue(1.0f)
				, mValue(1.0f)
			{
				mClip = WidgetClipping::Inherit;
			}

			~BuilderArguments() = default;		

			ARGUMENT_VALUE(float, MinValue)

			ARGUMENT_VALUE(float, MaxValue)

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(bool, IsFocusable)

			UI_EVENT(OnFloatValueChanged, OnValueChanged)

			ARGUMENT_ATTRIBUTE(Color, sliderBarColor)

			ARGUMENT_ATTRIBUTE(float, Value) //初始值
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseMove(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent) override;

		virtual bool supportsKeyboardFocus() const override;

		float getNormalizedValue() const;

		std::shared_ptr<Brush> getThumbImage() const;

		std::shared_ptr<Brush> getBarImage() const;

		void commitValue(float newValue);

		float positionToValue(const WidgetGeometry& geometry, const math::float2& absolutePosition);

		virtual bool isInteractable() const override;
	private:

		//持有滑条的方向
		Orientation m_orientation;

		std::shared_ptr<SliderStyle> m_sliderStyle;

		Attribute<Color> m_sliderBarColor;

		Attribute<float> m_valueAttribute;

		float m_minValue;
		float m_maxValue;
		OnFloatValueChanged m_onValueChanged;

		bool m_isFocusable;
	};
}