#pragma once

#include "Reply.h"
#include "Events.h"
#include "Clipping.h"
#include "WidgetGeometry.h"

#include <Core/Math/MyMath.h>

namespace GuGu{
    class Widget;
    class SlotBase;
    class ElementList;
    class ArrangedWidgetArray;
	struct PaintArgs
	{
		PaintArgs(std::vector<std::shared_ptr<Widget>>& inWidgets, double inCurrentTime, double inDeltaTime)
			: m_allWidgets(inWidgets)
            , m_currentTime(inCurrentTime)
            , m_deltaTime(inDeltaTime)
		{}

		std::vector<std::shared_ptr<Widget>>& m_allWidgets;

        double m_currentTime;
        
        double m_deltaTime;
	};
    class Widget : public std::enable_shared_from_this<Widget>
    {
    public:
        Widget();

        virtual ~Widget();

		uint32_t generateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual void cacheDesiredSize(float inLayoutScaleMultiplier);

        virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

        virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray);

        virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime);

        virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnKeyChar(const WidgetGeometry& myGeometry, const CharacterEvent& inCharacterEvent);

        virtual Reply OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent);

		virtual SlotBase* getSlot(uint32_t index);

		virtual uint32_t getSlotsNumber();

        virtual math::float2 getFixedSize() const;

        std::shared_ptr<Widget> getParentWidget() const;

        void setParentWidget(std::shared_ptr<Widget> parentWidget);

        const WidgetGeometry& getWidgetGeometry() const;

        uint32_t getLayer() const;

        void setFixedSize(math::float2 fixedSize);

        void prepass(float inLayoutScaleMultiplier);

        math::box2 calculateCullingAndClippingRules(const WidgetGeometry& allottedGeometry, const math::box2 cullingRect, bool& bClipToBounds);

        bool hasAnyFocus() const;
    protected:
        std::weak_ptr<Widget> m_parentWidget;
        WidgetGeometry m_geometry;
        math::float2 m_fixedSize;
        uint32_t m_layer;
        WidgetClipping m_widgetClipping;
    };
}