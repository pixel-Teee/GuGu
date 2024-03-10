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
		m_widgetClipping = arguments.mClip;
	}
	uint32_t Border::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
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

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	GuGu::math::float2 Border::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		//return m_childWidget->getChildWidget()->getFixedSize();
		return math::float2(m_imageBursh.Get()->m_actualSize.x, m_imageBursh.Get()->m_actualSize.y);
	}
	void Border::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one

		if (slotNumber)
		{
			AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
			AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

			const WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset), allocatedGeometry.getAccumulateTransform());

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