#pragma once

#include "WidgetGeometry.h"

namespace GuGu{
    class Slot;
    class ElementList;
    class ArrangedWidgetArray;
    class Widget{
    public:
        Widget();

        virtual ~Widget();

        virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

        virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray);

        virtual math::double2 getFixedSize();

        void setFixedSize(math::double2 fixedSize);

        virtual std::shared_ptr<Slot> getSlot(uint32_t index);

        virtual uint32_t getSlotsNumber();

        void prepass(float inLayoutScaleMultiplier);
    private:
        WidgetGeometry m_geometry;
        math::double2 m_fixedSize;
    };
}