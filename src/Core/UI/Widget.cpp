#include <pch.h>

#include "Widget.h"
#include "Slot.h"
#include "ElementList.h"
#include "ArrangedWidget.h"
#include "ToolTip.h"

#include <Application/Application.h>

#include <Renderer/Renderer.h>
#include "UIRenderPass.h"//move this

namespace GuGu{
    GuGuUtf8Str nameMouseButtonDown("MouseButtonDown");
    GuGuUtf8Str nameMouseButtonUp("MouseButtonUp");
    GuGuUtf8Str nameMouseButtonMove("MouseMove");

    Widget::Widget()
        : m_widgetClipping(WidgetClipping::Inherit)
    {
        m_layer = 0;
        m_hovered = false;
    }

    Widget::~Widget() {

    }

    uint32_t Widget::generateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
        Widget* mutableThis = const_cast<Widget*>(this);

        WidgetGeometry desktopSpaceGeometry = allocatedGeometry;
        desktopSpaceGeometry = desktopSpaceGeometry.getOffsetGeometry(paintArgs.getWindowToDesktopTransform());
        mutableThis->Tick(desktopSpaceGeometry, paintArgs.m_currentTime, paintArgs.m_deltaTime);

		paintArgs.m_allWidgets.push_back(shared_from_this());
		m_geometry = allocatedGeometry;
		m_layer = layer;

        bool bClipToBounds = false;
        bool bIntersectClipBounds = false;//这个控制是否受到父 clip 影响
        math::box2 cullingBounds = calculateCullingAndClippingRules(allocatedGeometry, cullingRect, bClipToBounds, bIntersectClipBounds);

        if (bClipToBounds)
        {
			ClippingZone clippingZone(allocatedGeometry);
			clippingZone.setShouldIntersectParent(bIntersectClipBounds);//是否和父亲的 clip 相交，得到一个更小的 clip 区域
			elementList.pushClip(clippingZone);
			//
			//ClippingZone clippingZone(allocatedGeometry);
			//std::vector<math::float2> points;
			//points.push_back(clippingZone.m_topLeft);
			//points.push_back(clippingZone.m_topRight);
			//points.push_back(clippingZone.m_bottomRight);
			//points.push_back(clippingZone.m_bottomLeft);
			//points.push_back(clippingZone.m_topLeft);
			//ElementList::addLineElement(elementList, WidgetGeometry(), math::float4(0.56f, 0.93f, 0.56f, 1.0f), points, 0.02f, layer + 1);

			//math::float2 direction = math::float2(700.0f, 410.0f);
			//ElementList::addSplineElement(elementList, WidgetGeometry(), math::float4(0.4f, 0.7f, 1.0f, 1.0f), clippingZone.m_topLeft, direction,
			//	clippingZone.m_bottomRight, direction, 2.7f, layer);
        } 

        UIRenderPass* uiRenderPass = Application::getApplication()->getRenderer()->getUIRenderPass();
        if (getType() + getLocation() == uiRenderPass->getSelectUINode())
        {
			ClippingZone clippingZone(allocatedGeometry);
            std::vector<math::float2> points;
            points.push_back(clippingZone.m_topLeft);
            points.push_back(clippingZone.m_topRight);
            points.push_back(clippingZone.m_bottomRight);
            points.push_back(clippingZone.m_bottomLeft);
            points.push_back(clippingZone.m_topLeft);
            ElementList::addLineElement(elementList, WidgetGeometry(), math::float4(0.56f, 0.93f, 0.56f, 1.0f), points, 0.02f, layer + 1);
        }

		//ClippingZone clippingZone(allocatedGeometry);
		//std::vector<math::float2> points;
		//points.push_back(clippingZone.m_topLeft);
		//points.push_back(clippingZone.m_topRight);
		//points.push_back(clippingZone.m_bottomRight);
		//points.push_back(clippingZone.m_bottomLeft);
		//points.push_back(clippingZone.m_topLeft);
		//ElementList::addLineElement(elementList, WidgetGeometry(), math::float4(0.56f, 0.93f, 0.56f, 1.0f), points, 0.02f, layer + 1);

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

    void Widget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const {
        //noting to do
    }
    void Widget::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
    {
    }

    Reply Widget::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (const PointerEventHandler* event = getPointerEvent(nameMouseButtonDown))
        {
            if (event->operator bool()) //is bound
            {
                return (*event)(myGeometry, inMouseEvent);
            }
        }
        return Reply::Unhandled();
    }
    Reply Widget::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
		if (const PointerEventHandler* event = getPointerEvent(nameMouseButtonUp))
		{
			if (event->operator bool()) //is bound
			{
				return (*event)(myGeometry, inMouseEvent);
			}
		}
		return Reply::Unhandled();
    }
    Reply Widget::OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
		if (const PointerEventHandler* event = getPointerEvent(nameMouseButtonMove))
		{
			if (event->operator bool()) //is bound
			{
				return (*event)(myGeometry, inMouseEvent);
			}
		}
		return Reply::Unhandled();
    }
    void Widget::OnMouseEnter(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        m_hovered = true;
    }
    void Widget::OnMouseLeave(const PointerEvent& inMouseEvent)
    {
        m_hovered = false;
    }
    Reply Widget::OnMouseWheel(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
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
    Reply Widget::OnKeyUp(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent)
    {
        return Reply::Unhandled();
    }
    Reply Widget::OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent)
    {
        return Reply::Unhandled();
    }
    void Widget::OnDragEnter(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
    {
        
    }
    Reply Widget::OnDragOver(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
    {
        return Reply::Unhandled();
    }
    void Widget::OnDragLeave(const DragDropEvent& dragDropEvent)
    {
    }
    Reply Widget::OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
    {
        return Reply::Unhandled();
    }
    void Widget::OnFocusChanging(const WeakWidgetPath& previousFocusPath, const WidgetPath& newWidgetPath)
    {
    }
    std::shared_ptr<PopupLayer> Widget::onVisualizePopup(const std::shared_ptr<Widget>& popupContent)
    {
        return std::shared_ptr<PopupLayer>();
    }
    PopupMethodReply Widget::OnQueryPopupMethod() const
    {
        return PopupMethodReply::unHandled();
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
    bool Widget::isInteractable() const
    {
        return false;
    }
    GuGu::SlotBase* Widget::getSlot(uint32_t index) const
	{
		return nullptr;
	}
	uint32_t Widget::getSlotsNumber() const
{
		return 0;
	}
    GuGu::math::float2 Widget::getFixedSize() const
{
        return m_fixedSize;
    }
    bool Widget::hasKeyboardFocus() const
    {
        return Application::getApplication()->getKeyboardFocusedWidget().get() == this;
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
                if(childWidget != nullptr)
                    childWidget->prepass(inLayoutScaleMultiplier);
			}
		}
        cacheDesiredSize(inLayoutScaleMultiplier);
    }
    math::box2 Widget::calculateCullingAndClippingRules(const WidgetGeometry& allottedGeometry, const math::box2 cullingRect, bool& bClipToBounds, bool& bIntersectClipBounds)
    {
        bClipToBounds = false;
        bIntersectClipBounds = true;//这个控制是否受到父 clip 影响

        switch (m_widgetClipping)
        {
        case WidgetClipping::Inherit:
            bClipToBounds = false;
            break;
        case WidgetClipping::ClipToBounds:
            bClipToBounds = true;
            break;
        case WidgetClipping::OnDemand://固定大小大于实际大小，会被裁剪
            const float overflowEpsilon = 1.0f;
            const math::float2& currentSize = getFixedSize();
            const math::float2& localSize = allottedGeometry.getLocalSize();
            bClipToBounds =
                (currentSize.x - overflowEpsilon) > localSize.x ||
                (currentSize.y - overflowEpsilon) > localSize.y;

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
    WidgetGeometry Widget::findChildGeometry(const WidgetGeometry& myGeometry, std::shared_ptr<Widget> widgetToFind) const
    {
        std::set<std::shared_ptr<Widget>> widgetsToFind;
        widgetsToFind.insert(widgetToFind);

        std::unordered_map<std::shared_ptr<Widget>, ArrangedWidget> result;
        //widgets to find 要找的 widget 集合
        findChildGeometries(myGeometry, widgetsToFind, result);
        auto it = result.find(widgetToFind);
        if (it != result.end())
            return it->second.getWidgetGeometry();
        return WidgetGeometry();
    }
    bool Widget::findChildGeometries(const WidgetGeometry& myGeometry, const std::set<std::shared_ptr<Widget>>& widgetsToFind, std::unordered_map<std::shared_ptr<Widget>, ArrangedWidget>& outResult) const
    {
        findChildGeometries_helper(myGeometry, widgetsToFind, outResult);
        return outResult.size() == widgetsToFind.size();
    }
    void Widget::findChildGeometries_helper(const WidgetGeometry& myGeometry, const std::set<std::shared_ptr<Widget>>& widgetsToFind, std::unordered_map<std::shared_ptr<Widget>, ArrangedWidget>& outResult) const
    {
        //执行bfs
        ArrangedWidgetArray arrangedChildren(Visibility::Visible);
        AllocationChildActualSpace(myGeometry, arrangedChildren);

        const int32_t numChildren = arrangedChildren.getArrangedWidgetsNumber();

        for (int32_t childIndex = 0; childIndex < numChildren; ++childIndex)
        {
            const ArrangedWidget& curChild = *arrangedChildren.getArrangedWidget(childIndex);

            if (widgetsToFind.find(curChild.getWidget()) != widgetsToFind.end())
            {
                outResult.insert({ curChild.getWidget(), curChild });
            }
        }

        if (outResult.size() != widgetsToFind.size())
        {
            for (int32_t childIndex = 0; childIndex < numChildren; ++childIndex)
            {
                const ArrangedWidget& curChild = *arrangedChildren.getArrangedWidget(childIndex);
                curChild.getWidget()->findChildGeometries_helper(curChild.getWidgetGeometry(), widgetsToFind, outResult);
            }
        }
    }
    void Widget::setDebugInfo(const GuGuUtf8Str& typeString, const GuGuUtf8Str& inFile, int32_t onLine)
    {
        m_widgetType = typeString;
        m_createLocation = inFile;
        m_line = onLine;
    }
    GuGuUtf8Str Widget::getLocation() const
    {
        return GuGuUtf8Str(m_createLocation + std::to_string(m_line));
    }
    GuGuUtf8Str Widget::getType() const
    {
        return m_widgetType;
    }
    bool Widget::IsHovered() const
    {
        return m_hovered;
    }
    WindowZone::Type Widget::getWindowZoneOverride() const
    {
        return WindowZone::Unspecified;
    }
    void Widget::setToolTipText(const Attribute<GuGuUtf8Str>& toolTipText)
    {
        m_toolTip = Application::getApplication()->makeToolTip(toolTipText);
    }
    void Widget::setToolTipText(const GuGuUtf8Str& toolTipText)
    {
        m_toolTip = Application::getApplication()->makeToolTip(toolTipText);
    }
    void Widget::setToolTip(const std::shared_ptr<IToolTip>& inToolTip)
    {
        m_toolTip = inToolTip;
    }

    std::shared_ptr<IToolTip> Widget::getToolTip() const
    {
        return m_toolTip;
    }

	const PointerEventHandler* Widget::getPointerEvent(const GuGuUtf8Str eventName) const
	{
		auto it = std::find_if(m_pointerEvents.begin(), m_pointerEvents.end(), [&eventName](const auto& testPair)
		{
			return testPair.first == eventName;
		});
        if (it != m_pointerEvents.end())
        {
            return &it->second;
        }
        return nullptr;
	}

	void Widget::setPointerEvent(const GuGuUtf8Str& eventName, PointerEventHandler& inEvent)
	{
        auto it = std::find_if(m_pointerEvents.begin(), m_pointerEvents.end(), [&eventName](const auto& testPair)
        {
            return testPair.first == eventName;
        });
        if (it != m_pointerEvents.end())
        {
            it->second = inEvent;
        }
        else
        {
            m_pointerEvents.push_back({eventName, inEvent});
        }
	}

	void Widget::setOnMouseButtonDown(PointerEventHandler eventHandler)
	{
        setPointerEvent(nameMouseButtonDown, eventHandler);
	}

	void Widget::setOnMouseButtonUp(PointerEventHandler eventHandler)
	{
        setPointerEvent(nameMouseButtonUp, eventHandler);
	}

	void Widget::setOnMouseMove(PointerEventHandler eventHandler)
	{
        setPointerEvent(nameMouseButtonMove, eventHandler);
	}

	void Widget::setClipping(WidgetClipping inClipping)
	{
        if (m_widgetClipping != inClipping)
        {
            m_widgetClipping = inClipping;
        }
	}

}