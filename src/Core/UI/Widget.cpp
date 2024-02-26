#include <pch.h>

#include "Widget.h"
#include "Slot.h"

namespace GuGu{

    Widget::Widget()
    {

    }

    Widget::~Widget() {

    }

    uint32_t Widget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        return layer;
    }

    math::double2 Widget::ComputeFixedSize(float inLayoutScaleMultiplier) {
        //noting to do
        return math::double2(0.0, 0.0);
    }

    void Widget::AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) {
        //noting to do
    }
    math::double2 Widget::getFixedSize()
    {
        return m_fixedSize;
    }
    void Widget::setFixedSize(math::double2 fixedSize)
    {
        m_fixedSize = fixedSize;
    }
    std::shared_ptr<Slot> Widget::getSlot(uint32_t index)
    {
        return nullptr;
    }
    uint32_t Widget::getSlotsNumber()
    {
        return 0;
    }
    void Widget::prepass(float inLayoutScaleMultiplier)
    {
		uint32_t widgetNumbers = getSlotsNumber();
		math::double2 size = math::double2(0.0, 0.0);
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<Slot> childSlot = getSlot(i);
			if (childSlot)
			{
				std::shared_ptr<Widget> childWidget = childSlot->getChildWidget();
                childWidget->prepass(inLayoutScaleMultiplier);
			}
		}
		size = ComputeFixedSize(inLayoutScaleMultiplier);
		setFixedSize(size);
    }
}