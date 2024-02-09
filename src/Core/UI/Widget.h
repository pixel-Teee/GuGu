#pragma once

#include "WidgetGeometry.h"

namespace GuGu{
    class Widget{
    public:
        Widget();

        virtual ~Widget();

        virtual void GenerateElement();

        virtual math::double2 ComputeFixedSize();

        virtual void AllocationChildActualSpace();

        virtual math::double2 getFixedSize();
    private:
        WidgetGeometry m_geometry;
        math::double2 m_fixedSize;
    };
}