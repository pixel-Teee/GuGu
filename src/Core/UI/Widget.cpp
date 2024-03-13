#include <pch.h>

#include "Widget.h"
#include "Slot.h"
#include "ElementList.h"

namespace GuGu{

    Widget::Widget()
        : m_widgetClipping(WidgetClipping::Inherit)
    {
        m_layer = 0;
    }

    Widget::~Widget() {

    }

    uint32_t Widget::generateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
		paintArgs.m_allWidgets.push_back(shared_from_this());
		m_geometry = allocatedGeometry;
		m_layer = layer;

        bool bClipToBounds = false;
        math::box2 cullingBounds = calculateCullingAndClippingRules(allocatedGeometry, cullingRect, bClipToBounds);

        if (bClipToBounds)
        {
            ClippingZone clippingZone(allocatedGeometry);
            elementList.pushClip(clippingZone);

			std::vector<math::float2> points;
            points.push_back(clippingZone.m_topLeft);
            points.push_back(clippingZone.m_topRight);
            points.push_back(clippingZone.m_bottomRight);
            points.push_back(clippingZone.m_bottomLeft);
            points.push_back(clippingZone.m_topLeft);
            ElementList::addLineElement(elementList, WidgetGeometry(), math::float4(0.9f, 0.3f, 0.2f, 1.0f), points, 1.0f, layer + 1);

            //math::float2 direction = math::float2(150.0f, 0.0f);
			//ElementList::addSplineElement(elementList, WidgetGeometry(), math::float4(1.0f, 1.0f, 1.0f, 1.0f), clippingZone.m_topLeft, direction,
			//	clippingZone.m_bottomRight, direction, 0.02f, layer);
        }

        //clipping
        uint32_t maxLayer = onGenerateElement(paintArgs, cullingBounds, elementList, allocatedGeometry, layer);

        if (bClipToBounds)
        {
            elementList.popClip();
        }

        return maxLayer;
    }

    uint32_t Widget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        return layer;
    }

    GuGu::math::float2 Widget::ComputeFixedSize(float inLayoutScaleMultiplier) {
        //noting to do
        return math::float2(0.0, 0.0);
    }

    void Widget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) {
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
    GuGu::math::float2 Widget::getFixedSize() const
{
        return m_fixedSize;
    }
    std::shared_ptr<Widget> Widget::getParentWidget() const
    {
        return m_parentWidget.lock();
    }
    void Widget::setParentWidget(std::shared_ptr<Widget> parentWidget)
    {
        m_parentWidget = parentWidget;
    }
    const WidgetGeometry& Widget::getWidgetGeometry() const
    {
        return m_geometry;
    }
    uint32_t Widget::getLayer() const
    {
        return m_layer;
    }
    void Widget::setFixedSize(math::float2 fixedSize)
    {
        m_fixedSize = fixedSize;
    }
    void Widget::prepass(float inLayoutScaleMultiplier)
    {
		uint32_t widgetNumbers = getSlotsNumber();
		math::float2 size = math::float2(0.0, 0.0);
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
    math::box2 Widget::calculateCullingAndClippingRules(const WidgetGeometry& allottedGeometry, const math::box2 cullingRect, bool& bClipToBounds)
    {
        bClipToBounds = false;

        switch (m_widgetClipping)
        {
        case WidgetClipping::Inherit:
            bClipToBounds = false;
            break;
        case WidgetClipping::ClipToBounds:
            bClipToBounds = true;
            break;
        }

        if (bClipToBounds)
        {
            math::box2 myCullingRect(allottedGeometry.getRenderBoundingRect());

            return myCullingRect;
        }

        return cullingRect;
    }
}