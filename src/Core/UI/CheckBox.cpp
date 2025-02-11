#include <pch.h>

#include "CheckBox.h"
#include "Slot.h"
#include "Border.h"
#include "BoxPanel.h"
#include "Overlay.h"
#include "ImageWidget.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"

#include "Style.h"
#include "CoreStyle.h"

namespace GuGu {
	CheckBox::CheckBox()
		: m_checkBoxStyle(CoreStyle::getStyleSet()->getStyle<CheckBoxStyle>("checkBox"))
	{
		m_isCheckboxChecked = CheckBoxState::Unchecked;
		m_bIsPressed = false;
		m_childWidget = std::make_shared<SingleChildSlot>();
	}
	CheckBox::~CheckBox()
	{
	}
	void CheckBox::init(const BuilderArguments& arguments)
	{
		m_childWidget->setChildWidget(
								WIDGET_NEW(HorizontalBox)
								+ HorizontalBox::Slot()
								.FixedWidth()
								.setVerticalAlignment(VerticalAlignment::Center)
								.setHorizontalAlignment(HorizontalAlignment::Center)
								(
									WIDGET_NEW(Overlay)
									+ Overlay::Slot()
									.setVerticalAlignment(VerticalAlignment::Stretch)
									.setHorizontalAlignment(HorizontalAlignment::Stretch)
									(
										WIDGET_NEW(ImageWidget)
										.brush(this, &CheckBox::onGetBackgroundImage)
									)
									+ Overlay::Slot()
									.setVerticalAlignment(VerticalAlignment::Stretch)
									.setHorizontalAlignment(HorizontalAlignment::Stretch)
									(
										WIDGET_NEW(ImageWidget)
										.brush(this, &CheckBox::onGetCheckImage)
									)
								)
								+ HorizontalBox::Slot()
								.FixedWidth()
								.setVerticalAlignment(VerticalAlignment::Center)
								(
									WIDGET_NEW(Border)
									.brush(CoreStyle::getStyleSet()->getNoBrush())
									.Content					
									(
										arguments.mContent->getChildWidget() //todo:fix his
									)
								)
						);
		m_checkBoxStyle = arguments.mcheckBoxStyle;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
		m_clickMethod = arguments.mclickMethod;
		m_onCheckStateChanged = arguments.monCheckStateChanged;
	}
	void CheckBox::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		uint32_t slotNumber = getSlotsNumber();

		if (slotNumber)
		{
			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}
		}
	}
	uint32_t CheckBox::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	math::float2 CheckBox::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
			}
		}
		return math::float2(0, 0);
	}
	Reply CheckBox::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
		{
			m_bIsPressed = true;

			ButtonClickMethod::Type inputClickMethod = getClickMethodFromInputType(inMouseEvent);

			if (inputClickMethod == ButtonClickMethod::MouseDown)
			{
				toggleCheckedState();

				//todo:play sound				

				return Reply::Handled().captureMouse(shared_from_this());
			}
			else
			{
				return Reply::Handled().captureMouse(shared_from_this()).setFocus(shared_from_this());
			}		
		}
		else if (inMouseEvent.getEffectingButton() == Keys::RightMouseButton)
		{
			return Reply::Handled();
		}
		else
		{
			return Reply::Unhandled();
		}
	}

	Reply CheckBox::OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		return OnMouseButtonDown(myGeometry, inMouseEvent);
	}

	Reply CheckBox::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
		{
			m_bIsPressed = false;
			
			if (m_clickMethod == ButtonClickMethod::MouseDown)
			{

			}
			else
			{
				//mouse up
				const bool isUnderMouse = geometry.isUnderLocation(inMouseEvent.m_screenSpacePosition);
				if (isUnderMouse)
				{
					if (m_clickMethod == ButtonClickMethod::MouseUp || hasMouseCapture())
					{
						toggleCheckedState();
						const CheckBoxState state = m_isCheckboxChecked.Get();
						if (state == CheckBoxState::Checked)
						{
							//play sound
						}
						else
						{
							//play sound
						}
					}
				}
			}
			return Reply::Unhandled().releaseMouseCapture();
		}
		
		return Reply::Unhandled();
	}
	bool CheckBox::isInteractable() const
	{
		return true;
	}
	SlotBase* CheckBox::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t CheckBox::getSlotsNumber() const
	{
		return 1;
	}
	std::shared_ptr<Brush> CheckBox::onGetBackgroundImage() const
	{
		return m_checkBoxStyle->m_backgroundImage;
	}
	std::shared_ptr<Brush> CheckBox::onGetCheckImage() const
	{
		CheckBoxState state = m_isCheckboxChecked.Get();

		std::shared_ptr<Brush> imageToUse;
		switch (state)
		{
		case GuGu::CheckBoxState::Unchecked:
			imageToUse = (IsHovered() ? getUncheckedHoveredImage() : getUncheckedImage());
			break;
		case GuGu::CheckBoxState::Checked:
			imageToUse = (IsHovered() ? getCheckedHoveredImage() : getCheckedImage());
			break;
		}
		return imageToUse;
	}

	std::shared_ptr<GuGu::Brush> CheckBox::getUncheckedHoveredImage() const
	{
		return m_checkBoxStyle->m_uncheckedHoveredImage;
	}

	std::shared_ptr<GuGu::Brush> CheckBox::getUncheckedImage() const
	{
		return m_checkBoxStyle->m_uncheckedImage;
	}

	std::shared_ptr<GuGu::Brush> CheckBox::getCheckedHoveredImage() const
	{
		return m_checkBoxStyle->m_checkedHoveredImage;
	}

	std::shared_ptr<GuGu::Brush> CheckBox::getCheckedImage() const
	{
		return m_checkBoxStyle->m_checkedImage;
	}

	void CheckBox::toggleCheckedState()
	{
		const CheckBoxState state = m_isCheckboxChecked.Get();

		if (state == CheckBoxState::Checked)
		{
			if (!m_isCheckboxChecked.IsBound())
			{
				m_isCheckboxChecked.Set(CheckBoxState::Unchecked);
			}

			if (m_onCheckStateChanged)
			{
				m_onCheckStateChanged(CheckBoxState::Unchecked);
			}
			
		}
		else if (state == CheckBoxState::Unchecked)
		{
			if (!m_isCheckboxChecked.IsBound())
			{
				m_isCheckboxChecked.Set(CheckBoxState::Checked);
			}

			if (m_onCheckStateChanged)
			{
				m_onCheckStateChanged(CheckBoxState::Checked);
			}
		}
	}

	EnumAsByte<ButtonClickMethod::Type> CheckBox::getClickMethodFromInputType(const PointerEvent& mouseEvent) const
	{
		return m_clickMethod;
	}

}