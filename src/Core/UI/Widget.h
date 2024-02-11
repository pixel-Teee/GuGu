#pragma once

#include "WidgetGeometry.h"

namespace GuGu{
    class Slot;
    class ElementList;
    class Widget{
    public:
        Widget();

        virtual ~Widget();

        virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual math::double2 ComputeFixedSize();

        virtual void AllocationChildActualSpace();

        virtual math::double2 getFixedSize();

        void setFixedSize(math::double2 fixedSize);

        virtual std::shared_ptr<Slot> getSlot(uint32_t index);

        virtual uint32_t getSlotsNumber();
    private:
        WidgetGeometry m_geometry;
        math::double2 m_fixedSize;
    };
}