#pragma once

#include "Reply.h"
#include "Events.h"
#include "WidgetGeometry.h"

namespace GuGu{
    class Widget;
    class SlotBase;
    class ElementList;
    class ArrangedWidgetArray;
	struct PaintArgs
	{
		PaintArgs(std::vector<std::shared_ptr<Widget>>& inWidgets)
			: m_allWidgets(inWidgets)
		{}

		std::vector<std::shared_ptr<Widget>>& m_allWidgets;
	};
    class Widget : public std::enable_shared_from_this<Widget>
    {
    public:
        Widget();

        virtual ~Widget();

        virtual uint32_t GenerateElement(PaintArgs& paintArgs, ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

        virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray);

        virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		virtual SlotBase* getSlot(uint32_t index);

		virtual uint32_t getSlotsNumber();

        virtual math::double2 getFixedSize() const;

        const WidgetGeometry& getWidgetGeometry() const;

        uint32_t getLayer() const;

        void setFixedSize(math::double2 fixedSize);

        void prepass(float inLayoutScaleMultiplier);
    protected:
        WidgetGeometry m_geometry;
        math::double2 m_fixedSize;
        uint32_t m_layer;
    };
}