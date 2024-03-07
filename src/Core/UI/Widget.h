#pragma once

#include "WidgetGeometry.h"

namespace GuGu{
    class SlotBase;
    class ElementList;
    class ArrangedWidgetArray;
    class Widget{
    public:
        Widget();

        virtual ~Widget();

        virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

        virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray);

		virtual SlotBase* getSlot(uint32_t index);

		virtual uint32_t getSlotsNumber();

        virtual math::double2 getFixedSize() const;

        void setFixedSize(math::double2 fixedSize);

        void prepass(float inLayoutScaleMultiplier);
    private:
        WidgetGeometry m_geometry;
        math::double2 m_fixedSize;
    };
}