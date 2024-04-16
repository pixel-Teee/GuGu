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
	
	template<class T>
	static constexpr T gridSnap(T location, T grid)
	{
		return (grid == T{}) ? location : (std::floor((location + (grid / (T)2)) * grid) * grid);
	}

	template<typename NumericType>
	class SpinBox : public Widget
	{
	public:
		using OnValueChanged = std::function<void(NumericType)>;

		using OnValueCommitted = std::function<void(NumericType, TextCommit::Type)>;

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

			UI_EVENT(OnValueChanged, onValueChanged)

			UI_EVENT(OnValueCommitted, onValueCommitted)

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

			m_valueAttribute = arguments.mValue;
			m_minValue = arguments.mMinValue;
			m_maxValue = arguments.mMaxValue;
			m_minFractionalDigits = arguments.mMinFractionDigits.Get().has_value() ? arguments.mMinFractionDigits : defaultMinFractionalDigits;
			m_maxFractionalDigits = arguments.mMaxFractionDigits.Get().has_value() ? arguments.mMaxFractionDigits : defaultMaxFractionalDigits;
			m_alwaysUsesDeltaSnap = arguments.mAlwaysUseDeltaSnap;

			m_cachedExternalValue = m_valueAttribute.Get();
			m_cachedValueString = m_interface->toString(m_cachedExternalValue);

			m_internalValue = (double)m_cachedExternalValue;

			m_distanceDragged = 0.0f;
			m_preDragValue = NumericType();
			m_bDragging = false;
			m_delta = arguments.mDelta;
			m_shiftMouseMovePixelPerDelta = arguments.mShiftMouseMovePixelPerDelta;
			m_linearDeltaSensitivity = arguments.mLinearDeltaSensitivity;
			m_sliderExponent = arguments.mSliderExponent;
			m_onValueChanged = arguments.monValueChanged;
			m_onValueCommitted = arguments.monValueCommitted;
			m_visibilityAttribute = arguments.mVisibility;

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
					.visibility(Visibility::Visible)
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
					.visibility(Visibility::Collapsed)
					.SelecAllTextWhenFocus(true) //当获取焦点的时候，选中所有文本
				)
			);

			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());		
		}

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override
		{
			ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
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
				const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

				if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
				{
					AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
					AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

					WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset), allocatedGeometry.getAccumulateTransform());

					arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
				}			
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

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			//todo:判断鼠标左键按下
			m_distanceDragged = 0.0f;
			m_preDragValue = m_valueAttribute.Get();
			m_internalValue = (double)m_preDragValue;
			m_cachedMousePosition = math::int2(inMouseEvent.m_screenSpacePosition.x, inMouseEvent.m_screenSpacePosition.y);

			Reply returnReply = Reply::Handled().captureMouse(shared_from_this()).setFocus(shared_from_this());

			return returnReply;
		}

		virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			if (!this->hasMouseCapture())
			{
				//lost the mouse capture
				m_bDragging = false;

				return Reply::Unhandled();
			}

			if (!m_bDragging)
			{
				m_distanceDragged += std::abs(inMouseEvent.m_cursorDelta.x);
				if (m_distanceDragged > 5.0f)
				{
					exitTextMode();//退出文本编辑模式
					m_bDragging = true;
				}

				m_cachedMousePosition = math::int2(inMouseEvent.m_screenSpacePosition.x, inMouseEvent.m_screenSpacePosition.y);
			}
			else
			{
				double newValue = 0.0f;

				//提升spin基于 delta mouse 移动

				//一个最小的slider宽度使用，用于计算deltas，在slider范围的空间的
				const float minSliderWidth = 100.0f;
				float sliderWidthInSlateUnits = std::max(myGeometry.getAbsoluteSize().x, minSliderWidth);

				const int32_t cachedShiftMouseMovePixelPerDelta = m_shiftMouseMovePixelPerDelta.Get();
				if (cachedShiftMouseMovePixelPerDelta > 1 && inMouseEvent.isShiftDown())
				{
					sliderWidthInSlateUnits *= cachedShiftMouseMovePixelPerDelta;
				}

				//todo:加入动态改变最大值和最小值的逻辑

				//todo:加入可调节的spin

				const float sign = (inMouseEvent.getCursorDelta().x > 0) ? 1.0 : -1.0f;

				if (m_linearDeltaSensitivity.IsSet() && m_linearDeltaSensitivity.Get() != 0 && m_delta.IsSet() && m_delta.Get() > 0)
				{
					//除以线性敏感度
					const float mouseDelta = std::abs(inMouseEvent.getCursorDelta().x / m_linearDeltaSensitivity.Get());
					newValue = m_internalValue + (sign * mouseDelta * std::pow((float)m_delta.Get(), m_sliderExponent.Get()));
				}
				else
				{
					const float mouseDelta = std::abs(inMouseEvent.getCursorDelta().x / sliderWidthInSlateUnits);
					const double currentValue = std::clamp(std::abs(m_internalValue), 1.0, (double)5000.0f);//todo：这里要修复，根据NumericType选取最大值
					newValue = m_internalValue + (sign * mouseDelta * std::pow(currentValue, m_sliderExponent.Get()));
				}
				
				NumericType roundedNewValue = newValue;//todo:如果是整数，要round

				commitValue(roundedNewValue, newValue, CommitedViaSpin, TextCommit::OnEnter);

				return Reply::Handled();
			}

			return Reply::Unhandled();
		}

		virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			//todo:判断左键按下

			if (!hasMouseCapture())
			{
				//lost capture
				m_bDragging = false;

				return Reply::Unhandled();
			}

			if (m_bDragging)
			{
				NumericType currentDelta = m_delta.Get();//提升value的delta
				if (currentDelta != NumericType())//如果提供了delta
				{
					//grid snap
					m_internalValue = gridSnap(m_internalValue, (double)currentDelta);//snap 这个 double 的internal value
				}

				//todo:实现一个round if integer value
				const NumericType currentValue = (double)m_internalValue;//todo:如果这是一个整数，进行round
				notifyValueCommitted(currentValue);
			}

			m_bDragging = false;

			Reply reply = Reply::Handled().releaseMouseCapture();

			if (m_distanceDragged < 5.0f)//todo:这里这个之后要修复
			{
				enterTextMode();//松开鼠标进入文本编辑模式
				reply.setFocus(m_editableText);
			}

			return reply;
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
			CommittedViaArrowKey,
			CommittedViaCode
		};

		void textFieldOnTextCommited(const GuGuUtf8Str& newText, TextCommit::Type commitInfo)
		{
			//todo:进入文本模式
			if (commitInfo != TextCommit::OnEnter)
			{
				exitTextMode();
			}

			std::optional<NumericType> newValue = m_interface->fromString(newText, m_valueAttribute.Get());
			if (newValue.has_value())
			{
				commitValue(newValue.value(), (double)newValue.value(), CommitedViaTypeIn, commitInfo);
			}
		}

		void commitValue(double newValue, double newSpinValue, CommitMethod commitMethod, TextCommit::Type originalCommitInfo)
		{
			if (commitMethod == CommitedViaSpin || commitMethod == CommittedViaArrowKey)
			{
				newValue = std::clamp(newValue, (double)getMinSliderValue(), (double)getMaxSliderValue());
				newSpinValue = std::clamp(newSpinValue, (double)getMinSliderValue(), (double)getMaxSliderValue());
			}

			newValue = std::clamp(newValue, (double)getMinValue(), (double)getMaxValue());
			newSpinValue = std::clamp(newSpinValue, (double)getMinValue(), (double)getMaxValue());

			if (!m_valueAttribute.IsBound())
			{
				m_valueAttribute.Set(newValue);//todo:这里要round到整数
			}

			if (commitMethod == CommitedViaSpin)
			{
				const NumericType currentValue = m_valueAttribute.Get();
				if (currentValue != m_cachedExternalValue)
				{
					newValue = currentValue;
					newSpinValue = (double)currentValue;
				}
			}

			m_internalValue = newSpinValue;

			if (commitMethod == CommitedViaTypeIn || commitMethod == CommittedViaArrowKey)
			{
				m_onValueCommitted(newValue, originalCommitInfo);
			}

			m_onValueChanged(newValue);

			//todo:增加更多逻辑

			if (!m_valueAttribute.IsBound())
			{
				m_valueAttribute.Set(newValue);
			}

			//更新用户所相信的外部的值
			const NumericType currentValue = m_valueAttribute.Get();
			if (m_cachedExternalValue != currentValue)
			{
				m_cachedExternalValue = m_valueAttribute.Get();
				m_cachedValueString = m_interface->toString(m_cachedExternalValue);
			}

			if (!this->hasMouseCapture())
			{
				m_bDragging = false;
			}
		}

		void notifyValueCommitted(NumericType currentValue) const
		{
			m_onValueCommitted(currentValue, TextCommit::OnEnter);
		}
	protected:
		void enterTextMode()
		{
			m_textBlock->setVisibility(Visibility::Collapsed);
			m_editableText->setVisibility(Visibility::Visible);
		}

		void exitTextMode()
		{
			m_textBlock->setVisibility(Visibility::Visible);
			m_editableText->setVisibility(Visibility::Collapsed);
		}

	private:
		static const int32_t defaultMinFractionalDigits;

		static const int32_t defaultMaxFractionalDigits;

		std::shared_ptr<SpinBoxStyle> m_spinBoxStyle;

		Attribute<NumericType> m_valueAttribute;

		//这是cache的值，用户相信这个，通常由于截断为int而有所不同
		//通常用于识别作用在spin box 上外部的值，并且同步内部的值到它们，当一个值提交到spin box 的时候，进行同步
		NumericType m_cachedExternalValue;

		//用于阻止逐帧的cached数值值的转换到字符串
		GuGuUtf8Str m_cachedValueString;

		double m_internalValue;

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
		Attribute<NumericType> m_delta;
		Attribute<bool> m_alwaysUsesDeltaSnap;
		Attribute<int32_t> m_shiftMouseMovePixelPerDelta;
		Attribute<int32_t> m_linearDeltaSensitivity;
		Attribute<float> m_sliderExponent;

		//cache 的鼠标位置，去恢复，在滚动之后
		math::int2 m_cachedMousePosition;

		//用户是否在拖动滑条
		bool m_bDragging;

		float m_distanceDragged;

		//internal value 的状态，在一个drag操作开始之前
		NumericType m_preDragValue;

		OnValueCommitted m_onValueCommitted;

		OnValueChanged m_onValueChanged;
	};

	template<typename NumericType>
	const int32_t SpinBox<NumericType>::defaultMinFractionalDigits = 1;

	template<typename NumericType>
	const int32_t SpinBox<NumericType>::defaultMaxFractionalDigits = 6;
}