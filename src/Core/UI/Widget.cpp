#include <pch.h>

#include "Widget.h"

namespace GuGu{

    Widget::Widget()
    {

    }

    Widget::~Widget() {

    }

    void Widget::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
    }

    math::double2 Widget::ComputeFixedSize() {
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
}