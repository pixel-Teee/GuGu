#include <pch.h>

#include "Widget.h"
#include "Slot.h"

namespace GuGu{

    Widget::Widget()
    {
        m_layer = 0;
    }

    Widget::~Widget() {

    }

    uint32_t Widget::GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
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
    Reply Widget::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        return Reply::Unhandled();
    }
    SlotBase* Widget::getSlot(uint32_t index)
	{
		return nullptr;
	}
	uint32_t Widget::getSlotsNumber()
	{
		return 0;
	}
    math::double2 Widget::getFixedSize() const
    {
        return m_fixedSize;
    }
    const WidgetGeometry& Widget::getWidgetGeometry() const
    {
        return m_geometry;
    }
    uint32_t Widget::getLayer() const
    {
        return m_layer;
    }
    void Widget::setFixedSize(math::double2 fixedSize)
    {
        m_fixedSize = fixedSize;
    }
    void Widget::prepass(float inLayoutScaleMultiplier)
    {
		uint32_t widgetNumbers = getSlotsNumber();
		math::double2 size = math::double2(0.0, 0.0);
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			SlotBase* childSlot = getSlot(i);
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