#include <pch.h>

#include "Widget.h"
#include "Slot.h"
#include "ElementList.h"

#include <Application/Application.h>

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
        Widget* mutableThis = const_cast<Widget*>(this);

        mutableThis->Tick(allocatedGeometry, paintArgs.m_currentTime, paintArgs.m_deltaTime);

		paintArgs.m_allWidgets.push_back(shared_from_this());
		m_geometry = allocatedGeometry;
		m_layer = layer;

        bool bClipToBounds = false;
        math::box2 cullingBounds = calculateCullingAndClippingRules(allocatedGeometry, cullingRect, bClipToBounds);

        if (bClipToBounds)
        {
			ClippingZone clippingZone(allocatedGeometry);
			elementList.pushClip(clippingZone);
			
			//std::vector<math::float2> points;
			//points.push_back(clippingZone.m_topLeft);
			//points.push_back(clippingZone.m_topRight);
			//points.push_back(clippingZone.m_bottomRight);
			//points.push_back(clippingZone.m_bottomLeft);
			//points.push_back(clippingZone.m_topLeft);
			//ElementList::addLineElement(elementList, WidgetGeometry(), math::float4(0.56f, 0.93f, 0.56f, 1.0f), points, 1.0f, layer + 1);
			//
			//math::float2 direction = math::float2(700.0f, 410.0f);
			//ElementList::addSplineElement(elementList, WidgetGeometry(), math::float4(0.4f, 0.7f, 1.0f, 1.0f), clippingZone.m_topLeft, direction,
			//	clippingZone.m_bottomRight, direction, 2.7f, layer);
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

    void Widget::cacheDesiredSize(float inLayoutScaleMultiplier)
    {
        m_fixedSize = ComputeFixedSize(inLayoutScaleMultiplier);
    }

    GuGu::math::float2 Widget::ComputeFixedSize(float inLayoutScaleMultiplier) {
        //noting to do
        return math::float2(0.0, 0.0);
    }

    void Widget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) {
        //noting to do
    }
    void Widget::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
    {
    }

    Reply Widget::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        //GuGu_LOGD("{%f, %f}", inMouseEvent.m_screenSpacePosition.x, inMouseEvent.m_screenSpacePosition.y);
        return Reply::Unhandled();
    }
    Reply Widget::OnKeyChar(const WidgetGeometry& myGeometry, const CharacterEvent& inCharacterEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
    {
        return Reply::Unhandled();
    }
    void Widget::OnFocusLost()
    {
    }
    Reply Widget::OnFocusReceived(const WidgetGeometry& myGeometry)
    {
        return Reply::Unhandled();
    }
    bool Widget::supportsKeyboardFocus() const
    {
        return false;
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
        cacheDesiredSize(inLayoutScaleMultiplier);
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
    bool Widget::hasAnyFocus() const
    {
        return Application::getApplication()->hasAnyFocus(shared_from_this());
    }
    bool Widget::hasMouseCapture() const
    {
        return Application::getApplication()->doesWidgetHaveMouseCapture(shared_from_this());
    }
}