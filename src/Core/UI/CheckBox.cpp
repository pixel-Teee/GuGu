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
#include "StyleSet.h"

namespace GuGu {
	CheckBox::CheckBox()
		: m_checkBoxStyle(StyleSet::getStyle()->getStyle<CheckBoxStyle>("checkBox"))
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
									.brush(StyleSet::getStyle()->getNoBrush())
									.Content					
									(
										arguments.mContent->getChildWidget() //todo:fix his
									)
								)
						);
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
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
		m_bIsPressed = true;

		if (m_isCheckboxChecked.Get() == CheckBoxState::Checked)
		{
			m_isCheckboxChecked = CheckBoxState::Unchecked;
		}
		else if(m_isCheckboxChecked.Get() == CheckBoxState::Unchecked)
		{
			m_isCheckboxChecked = CheckBoxState::Checked;
		}

		return Reply::Unhandled().captureMouse(shared_from_this());
	}
	Reply CheckBox::OnMouseButtonUp(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
	{
		m_bIsPressed = false;

		//if (m_isCheckboxChecked.Get() == CheckBoxState::Checked)
		//{
		//	m_isCheckboxChecked = CheckBoxState::Unchecked;
		//}
		//else if (m_isCheckboxChecked.Get() == CheckBoxState::Unchecked)
		//{
		//	m_isCheckboxChecked = CheckBoxState::Checked;
		//}
		//
		return Reply::Unhandled().releaseMouseCapture();
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
			imageToUse = m_checkBoxStyle->m_uncheckedImage;
			break;
		case GuGu::CheckBoxState::Checked:
			imageToUse = m_checkBoxStyle->m_checkedImage;
			break;
		}
		return imageToUse;
	}
}