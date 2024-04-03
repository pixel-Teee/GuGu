#pragma once


#include "Slot.h"
#include "Widget.h"
#include "TextLayout.h"//TextJustify::Type
#include "EditableText.h"
#include "TextBlockWidget.h"
#include "NumericTypeInterface.h"
#include "Style.h"
#include "StyleSet.h"
#include "UIMacros.h"
#include "BoxPanel.h"
#include "LayoutUtils.h"
#include "ElementList.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	
	template<typename NumericType>
	class SpinBox : public Widget
	{
	public:
		struct BuilderArguments : public Arguments<SpinBox<NumericType>>
		{
			BuilderArguments()
				: mStyle(StyleSet::getStyle()->template getStyle<SpinBoxStyle>("SpinBox"))
				, mValue(0)
				, mMinFractionDigits(defaultMinFractionalDigits)
				, mMaxFractionDigits(defaultMaxFractionalDigits)
				, mAlwaysUseDeltaSnap(false)
				, mDelta(0)
				, mShiftMouseMovePixelPerDelta(1)
				, mSupportDynamicSliderMinValue(false)
				, mSupportDynamicSliderMaxValue(false)
				, mSliderExponent(1.0f)
				, mMinDesiredWidth(0.0f)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<SpinBoxStyle>, Style)

			//要去显示的值
			ARGUMENT_ATTRIBUTE(NumericType, Value)

			//最小可以输入文本编辑盒的值
			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, MinValue)

			//最大可以输入文本编辑盒的值
			ARGUMENT_ATTRIBUTE(std::optional<NumericType>, MaxValue)

			//最小的小数位数spix box可以显示，默认为1
			ARGUMENT_ATTRIBUTE(std::optional<int32_t>, MinFractionDigits)

			//最大的小数位数spin box可以显示，默认为6
			ARGUMENT_ATTRIBUTE(std::optional<int32_t>, MaxFractionDigits)

			//是否输入的值应当使用delta snapping ，默认为false
			ARGUMENT_ATTRIBUTE(bool, AlwaysUseDeltaSnap)

			//提升value的delta，当slider移动的时候，如果没有指定，将自动的决定
			ARGUMENT_ATTRIBUTE(NumericType, Delta)

			//鼠标需要移动多少像素才能改变delta snap 的值
			ARGUMENT_ATTRIBUTE(int32_t, ShiftMouseMovePixelPerDelta)

			//如果我们是一个未绑定的spin box ，在被delta操作之前，什么值我们除以鼠标移动，要求delta被设置
			ARGUMENT_ATTRIBUTE(int32_t, LinearDeltaSensitivity)

			//告诉我们，是否我们想去最大值动态的修改，使用ctrl
			ARGUMENT_ATTRIBUTE(bool, SupportDynamicSliderMaxValue)

			//告诉我们，是否我们想去最小值动态的修改，使用ctrl
			ARGUMENT_ATTRIBUTE(bool, SupportDynamicSliderMinValue)

			//使用指数缩放，对于滑条
			ARGUMENT_ATTRIBUTE(float, SliderExponent)

			//当使用指数缩放，对于滑条，什么是自然值
			ARGUMENT_ATTRIBUTE(NumericType, SliderExponentNeutralValue)

			//一个spin box 最小的宽度
			ARGUMENT_ATTRIBUTE(float, MinDesiredWidth)

			//spin box 中值的对齐方式
			ARGUMENT_ATTRIBUTE(TextJustify::Type, Justification)
		};

		SpinBox() {}

		virtual ~SpinBox() {}

		void init(const BuilderArguments& arguments)
		{
			m_spinBoxStyle = arguments.mStyle.Get();

			m_interface = std::make_shared<DefaultNumericTypeInterface<NumericType>>();
			m_minValue = arguments.mMinValue;
			m_maxValue = arguments.mMaxValue;

			this->m_childWidget = std::make_shared<SingleChildSlot>();
			//m_childWidget = arguments.mContent;
			this->m_childWidget->setChildWidget(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.StretchWidth(1.0f)
				.setHorizontalAlignment(HorizontalAlignment::Stretch)
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_ASSIGN_NEW(TextBlockWidget, m_textBlock)
					.text(this, &SpinBox<NumericType>::getValueAsText) //todo: 添加文本的最小宽度和对齐方式
				)
				+ HorizontalBox::Slot()
				.StretchWidth(1.0f)
				.setHorizontalAlignment(HorizontalAlignment::Stretch)
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_ASSIGN_NEW(EditableText, m_editableText)
					.text(this, &SpinBox<NumericType>::getValueAsText)
					.onIsTypedCharValid(this, &SpinBox<NumericType>::isCharacterValid)
					.onTextCommitted(this, &SpinBox<NumericType>::textFieldOnTextCommited)
				)
			);

			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());

			m_minFractionalDigits = arguments.mMinFractionDigits.Get().has_value() ? arguments.mMinFractionDigits : defaultMinFractionalDigits;
			m_maxFractionalDigits = arguments.mMaxFractionDigits.Get().has_value() ? arguments.mMaxFractionDigits : defaultMaxFractionalDigits;
		}

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override
		{
			ArrangedWidgetArray arrangedWidgetArray;
			AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

			ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_spinBoxStyle->m_backgroundBrsuh, layer + 1); //background			

			uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
			//math::double2 size = math::double2(0.0, 0.0);
			uint32_t maxLayer = 0;
			for (size_t i = 0; i < widgetNumbers; ++i)
			{
				std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
				if (childWidget)
				{
					std::shared_ptr<Widget> widget = childWidget->getWidget();

					maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 2));
				}
			}

			return maxLayer;
		}

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override
		{
			return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
		}

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override
		{
			//arrange single children
			uint32_t slotNumber = getSlotsNumber();
			//check this is one
			//assert(slotNumber == 1);

			if (slotNumber)
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset), allocatedGeometry.getAccumulateTransform());

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}
		}

		virtual SlotBase* getSlot(uint32_t index) override
		{
			return m_childWidget.get();
		}

		virtual uint32_t getSlotsNumber() override
		{
			return 1;
		}

		GuGuUtf8Str getValueAsText() const
		{
			NumericType currentValue = m_valueAttribute.Get();
			if (currentValue == m_cachedExternalValue)
			{
				return m_cachedValueString;
			}

			return m_interface->toString(currentValue);
		}

		//todo:这里要根据NumericType类型来获取最小值
		NumericType getMinSliderValue() const { return m_minSliderValue.Get().value_or(-5000.0f);}
		NumericType getMaxSliderValue() const { return m_maxSliderValue.Get().value_or(5000.0f); }
		NumericType getMinValue() const { return m_minValue.Get().value_or(-5000.0f); }
		NumericType getMaxValue() const { return m_maxValue.Get().value_or(5000.0f); } //todo:这里转换为NumericType，如果不匹配，会崩溃
		
		bool isCharacterValid(GuGuUtf8Str inChar) const
		{
			return m_interface->isCharacterValid(inChar);
		}

		enum CommitMethod
		{
			CommitedViaSpin,
			CommitedViaTypeIn,
			CommittedViaArrayKey,
			CommittedViaCode
		};

		void textFieldOnTextCommited(const GuGuUtf8Str& newText, TextCommit::Type commitInfo)
		{
			//todo:进入文本模式

			std::optional<NumericType> newValue = m_interface->fromString(newText, m_valueAttribute.Get());
			if (newValue.has_value())
			{
				commitValue(newValue.value(), CommitedViaTypeIn, commitInfo);
			}
		}

		void commitValue(double newValue, CommitMethod commitMethod, TextCommit::Type originalCommitInfo)
		{
			if (commitMethod == CommitedViaSpin || commitMethod == CommittedViaArrayKey)
			{
				newValue = std::clamp(newValue, (double)getMinSliderValue(), (double)getMaxSliderValue());
			}

			newValue = std::clamp(newValue, (double)getMinValue(), (double)getMaxValue());

			if (!m_valueAttribute.IsBound())
			{
				m_valueAttribute.Set(newValue);//todo:这里要round到整数
			}

			auto currentValue = m_valueAttribute.Get();

			if (commitMethod == CommitedViaSpin)
			{
				if (currentValue != m_cachedExternalValue)
				{
					newValue = currentValue;
				}
			}

			m_internalValue = newValue;

			//todo:增加更多逻辑


		}

	private:
		static const int32_t defaultMinFractionalDigits;

		static const int32_t defaultMaxFractionalDigits;

		std::shared_ptr<SpinBoxStyle> m_spinBoxStyle;

		Attribute<NumericType> m_valueAttribute;

		//这是cache的值，用户相信这个，通常由于截断为int而有所不同
		//通常用于识别作用在spin box 上外部的值，并且同步内部的值到它们，当一个值提交到spin box 的时候，进行同步
		NumericType m_cachedExternalValue;

		double m_internalValue;

		//用于阻止逐帧的cached数值值的转换到字符串
		GuGuUtf8Str m_cachedValueString;

		std::shared_ptr<SingleChildSlot> m_childWidget;

		std::shared_ptr<TextBlockWidget> m_textBlock;

		std::shared_ptr<EditableText> m_editableText;

		std::shared_ptr<INumericTypeInterface<NumericType>> m_interface;

		Attribute<std::optional<int32_t>> m_minFractionalDigits;
		Attribute<std::optional<int32_t>> m_maxFractionalDigits;
		Attribute<std::optional<NumericType>> m_minSliderValue;
		Attribute<std::optional<NumericType>> m_maxSliderValue;
		Attribute<std::optional<NumericType>> m_minValue;
		Attribute<std::optional<NumericType>> m_maxValue;
	};

	template<typename NumericType>
	const int32_t SpinBox<NumericType>::defaultMinFractionalDigits = 1;

	template<typename NumericType>
	const int32_t SpinBox<NumericType>::defaultMaxFractionalDigits = 6;
}