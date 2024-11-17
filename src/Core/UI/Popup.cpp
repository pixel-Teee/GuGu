#include <pch.h>
#include "Popup.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	Popup::Popup()
	{
	}
	Popup::~Popup()
	{
	}
	void Popup::init(const BuilderArguments& arguments)
	{
		m_visibilityAttribute = arguments.mVisibility;
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}
	uint32_t Popup::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
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
	math::float2 Popup::ComputeFixedSize(float inLayoutScaleMultiplier)
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
	void Popup::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
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
	SlotBase* Popup::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t Popup::getSlotsNumber() const
	{
		if (m_childWidget) return 1;
		return 0;
	}
}