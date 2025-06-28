#pragma once

#include "CompoundWidget.h"
#include "BoxPanel.h"
#include "UIMacros.h"
#include "SpinBox.h"
#include "WidgetDelegates.h"
#include "Overlay.h"
#include "NullWidget.h"

namespace GuGu {
	template<typename NumericType>
	class NumericEntryBox : public CompoundWidget
	{
	public:
		using OnValueChanged = std::function<void(NumericType)>;

		using OnValueCommitted = std::function<void(NumericType, TextCommit::Type)>;

		using OnDyamicSliderMinMaxValueChanged = std::function<void(NumericType, std::weak_ptr<Widget>, bool, bool)>;

		struct BuilderArguments : public Arguments<NumericEntryBox>
		{
			BuilderArguments()
			: mspinBoxStyle(CoreStyle::getStyleSet()->template getStyle<SpinBoxStyle>("SpinBox"))
			, mlabel()
			, mlabelVAlign(VerticalAlignment::Stretch)
			, mlabelPadding(Padding(3.0f, 0.0f))
			, mallowSpain(false)
			, mshiftMouseMovePixelPerDelta(1)
			, msupportDynamicSliderMaxValue(false)
			, msupportDynamicSliderMinValue(false)
			, mdelta(NumericType(0))
			, mminValue(std::numeric_limits<NumericType>::min())
			, mmaxValue(std::numeric_limits<NumericType>::max())
			, mminSliderValue(NumericType(-100))
			, mmaxSliderValue(NumericType(100))
			, msliderExponent(1.0f)
			, mminDesiredValueWidth(0.0f)
			{}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(VerticalAlignment, labelVAlign)

			ARGUMENT_VALUE(Padding, labelPadding)

			ARGUMENT_VALUE(bool, allowSpain)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, label)

			UI_EVENT(OnValueChanged, onValueChanged)

			UI_EVENT(OnValueCommitted, onValueCommitted)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<SpinBoxStyle>, spinBoxStyle)

			//当滑条滑动的时候，增加的值，如果不指定，则会自动决定
			ARGUMENT_ATTRIBUTE(NumericType, delta)

			ARGUMENT_ATTRIBUTE(int32_t, linearDeltaSensitivity)

			ARGUMENT_ATTRIBUTE(int32_t, shiftMouseMovePixelPerDelta)

			ARGUMENT_ATTRIBUTE(bool, supportDynamicSliderMaxValue)

			ARGUMENT_ATTRIBUTE(bool, supportDynamicSliderMinValue)

			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, minValue)

			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, maxValue)

			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, minSliderValue)

			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, maxSliderValue)

			ARGUMENT_ATTRIBUTE(float, sliderExponent)

			ARGUMENT_ATTRIBUTE(NumericType, sliderExponentNeutralValue)

			ARGUMENT_ATTRIBUTE(float, minDesiredValueWidth)

			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, value)
		};

		void init(const BuilderArguments& inArgs)
		{
			m_onValueChanged = inArgs.monValueChanged;
			m_onValueComitted = inArgs.monValueCommitted;
			m_valueAttribute = inArgs.mvalue;

			std::shared_ptr<Widget> labelWidget;
			if (inArgs.mlabel != nullptr)
			{
				labelWidget = inArgs.mlabel->getChildWidget();
			}
			else
			{
				labelWidget = NullWidget::getNullWidget();
			}

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
			
			const bool bAllowSpin = inArgs.mallowSpain;
			if (bAllowSpin)
			{
				WIDGET_ASSIGN_NEW(SpinBox<NumericType>, m_spinBox)
				.Style(inArgs.mspinBoxStyle)
				.Value(this, &NumericEntryBox<NumericType>::onGetValueForSpinBox)//要去显示的值
				.Delta(inArgs.mdelta)
				.LinearDeltaSensitivity(inArgs.mlinearDeltaSensitivity)
				.ShiftMouseMovePixelPerDelta(inArgs.mshiftMouseMovePixelPerDelta)
				.SupportDynamicSliderMaxValue(inArgs.msupportDynamicSliderMaxValue)
				.SupportDynamicSliderMinValue(inArgs.msupportDynamicSliderMinValue)
				.onValueChanged(inArgs.monValueChanged)
				.onValueCommitted(inArgs.monValueCommitted)
				//.minSliderValue(inArgs.mminSliderValue)
				//.maxSliderValue(inArgs.mmaxSliderValue)
				.MinValue(inArgs.mminValue)
				.MaxValue(inArgs.mmaxValue)
				.SliderExponent(inArgs.msliderExponent)
				.SliderExponentNeutralValue(inArgs.msliderExponentNeutralValue)
				.MinDesiredWidth(inArgs.mminDesiredValueWidth);
			}

			std::shared_ptr<Widget> mainContents;//主内容

			std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay);

			if (bAllowSpin)
			{
				overlay->addSlot()
				.setHorizontalAlignment(HorizontalAlignment::Stretch)
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					m_spinBox	
				);
			}

			if (bAllowSpin)
			{
				mainContents = overlay;
			}

			{
				std::shared_ptr<HorizontalBox> horizontalBox = WIDGET_NEW(HorizontalBox);

				//1.第一个label
				horizontalBox->addSlot()
				.FixedWidth()
				.setHorizontalAlignment(HorizontalAlignment::Left)
				.setVerticalAlignment(inArgs.mlabelVAlign)
				.setPadding(inArgs.mlabelPadding)
				(
					labelWidget
				);

				horizontalBox->addSlot()
				.FixedWidth()
				(
					mainContents
				);

				m_childWidget->m_childWidget = horizontalBox;
			}
		}


	private:
		NumericType onGetValueForSpinBox() const
		{
			const auto& value = m_valueAttribute.Get();

			if (value.has_value())
			{
				return value.value();
			}

			return 0;
		}
		OnValueChanged m_onValueChanged;
		OnValueCommitted m_onValueComitted;

		std::shared_ptr<Widget> m_spinBox;

		Attribute<std::optional<NumericType>> m_valueAttribute;
	};

}