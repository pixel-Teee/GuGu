#include <pch.h>

#include "Widget.h"

namespace GuGu{

    Widget::Widget()
    {

    }

    Widget::~Widget() {

    }

    void Widget::GenerateElement()
    {
    }

    math::double2 Widget::ComputeFixedSize() {
        //noting to do
        return math::double2(0.0, 0.0);
    }

    void Widget::AllocationChildActualSpace() {
        //noting to do
    }
    math::double2 Widget::getFixedSize()
    {
        return m_fixedSize;
    }
}