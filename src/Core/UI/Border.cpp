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
		m_childWidget = arguments.mContent;
		m_childWidget->setHorizontalAlignment(arguments.mhorizontalAlignment);
		m_childWidget->setVerticalAlignment(arguments.mverticalAlignment);
		m_childWidget->setPadding(arguments.mpadding);
	}
	uint32_t Border::GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		paintArgs.m_allWidgets.push_back(shared_from_this());
		m_geometry = allocatedGeometry;
		m_layer = layer;
		ArrangedWidgetArray arrangedWidgetArray;
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_imageBursh.Get(), layer); //background

		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->GenerateElement(paintArgs, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	math::double2 Border::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		//return m_childWidget->getChildWidget()->getFixedSize();
		return math::double2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
	}
	void Border::AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one

		if (slotNumber)
		{
			AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
			AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

			WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::double2(xalignmentResult.m_size, yAlignmentResult.m_size), math::double2(xalignmentResult.m_offset, yAlignmentResult.m_offset), allocatedGeometry.getAccumulateTransform());

			arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
		}

	}
	SlotBase* Border::getSlot(uint32_t index)
	{
		return m_childWidget.get();
	}
	uint32_t Border::getSlotsNumber()
	{
		return 1;
	}
}