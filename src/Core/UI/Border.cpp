#include <pch.h>

#include "Border.h"

#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "ElementList.h"

namespace GuGu {
	Border::Border()
	{
	}
	Border::~Border()
	{
	}
	void Border::init(const BuilderArguments& arguments)
	{
		m_imageBursh = arguments.mbrush;
		m_borderBackgroundColor = arguments.mBorderBackgroundColor;
		m_childWidget = arguments.mContent;
		m_childWidget->setHorizontalAlignment(arguments.mhorizontalAlignment);
		m_childWidget->setVerticalAlignment(arguments.mverticalAlignment);
		m_childWidget->setPadding(arguments.mpadding);
		m_widgetClipping = arguments.mClip;
		m_childWidget->m_parentWidget = shared_from_this();
		if(m_childWidget->m_childWidget != nullptr)
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t Border::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		if(m_imageBursh.Get() != nullptr && m_imageBursh.Get()->m_drawAs != BrushDrawType::NoDrawType)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, m_borderBackgroundColor.Get(), m_imageBursh.Get(), layer); //background
		}
		
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
	GuGu::math::float2 Border::ComputeFixedSize(float inLayoutScaleMultiplier)
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
	void Border::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one

		if (slotNumber)
		{
			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				const WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}			
		}

	}
	GuGu::SlotBase* Border::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t Border::getSlotsNumber() const 
{
		return 1;
	}
}