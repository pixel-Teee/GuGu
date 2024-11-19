#pragma once

#include "Reply.h"
#include "Events.h"
#include "Clipping.h"
#include "Visibility.h"
#include "WidgetGeometry.h"
#include "Attribute.h"
#include "ArrangedWidget.h"
#include "PopupMethodReply.h"

#include <Core/Math/MyMath.h>

#include <unordered_map>

#include <Core/Math/MyMath.h>

namespace GuGu{
    class Widget;
    class SlotBase;
    class ElementList;
    class ArrangedWidgetArray;
	struct PaintArgs
	{
		PaintArgs(std::vector<std::shared_ptr<Widget>>& inWidgets, math::float2 inWindowOffset, double inCurrentTime, double inDeltaTime)
			: m_allWidgets(inWidgets)
            , m_windowOffset(inWindowOffset)
            , m_currentTime(inCurrentTime)
            , m_deltaTime(inDeltaTime)
		{}

        math::float2 getWindowToDesktopTransform() { return m_windowOffset; }

		std::vector<std::shared_ptr<Widget>>& m_allWidgets;

        double m_currentTime;
        
        double m_deltaTime;

        math::float2 m_windowOffset;
	};
    class PopupLayer : public std::enable_shared_from_this<PopupLayer>
    {
    public:
        PopupLayer(const std::shared_ptr<Widget>& initHostWidget, const std::shared_ptr<Widget>& initPopupContent)
            : m_hostWidget(initHostWidget)
            , m_popupContent(initPopupContent)
        {}

        virtual ~PopupLayer() {}

        virtual std::shared_ptr<Widget> getHost() { return m_hostWidget; }
        virtual std::shared_ptr<Widget> getContent() { return m_popupContent; }
        virtual math::box2 getAbsoluteClientRect() = 0;
        virtual void remove() = 0;
    private:
        std::shared_ptr<Widget> m_hostWidget;
        std::shared_ptr<Widget> m_popupContent;
    };
    class WeakWidgetPath;
    class WidgetPath;
    class Widget : public std::enable_shared_from_this<Widget>
    {
    public:
        Widget();

        virtual ~Widget();

		uint32_t generateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

        virtual void cacheDesiredSize(float inLayoutScaleMultiplier);

        virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

        virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const;

        virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime);

        virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual void OnMouseEnter(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

        virtual void OnMouseLeave(const PointerEvent& inMouseEvent);

        virtual Reply OnKeyChar(const WidgetGeometry& myGeometry, const CharacterEvent& inCharacterEvent);

        virtual Reply OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent);

        virtual void OnFocusChanging(const WeakWidgetPath& previousFocusPath, const WidgetPath& newWidgetPath);

        virtual std::shared_ptr<PopupLayer> onVisualizePopup(const std::shared_ptr<Widget>& popupContent);

        virtual PopupMethodReply OnQueryPopupMethod() const;

        virtual void OnFocusLost();

        virtual Reply OnFocusReceived(const WidgetGeometry& myGeometry);

        virtual bool supportsKeyboardFocus() const;

        virtual bool isInteractable() const;//碰撞检测会使用这个

		virtual SlotBase* getSlot(uint32_t index) const;

		virtual uint32_t getSlotsNumber() const;

        virtual math::float2 getFixedSize() const;

        std::shared_ptr<Widget> getParentWidget() const;

        void setParentWidget(std::shared_ptr<Widget> parentWidget);

        const WidgetGeometry& getWidgetGeometry() const;

        uint32_t getLayer() const;

        void setFixedSize(math::float2 fixedSize);

        void prepass(float inLayoutScaleMultiplier);

        math::box2 calculateCullingAndClippingRules(const WidgetGeometry& allottedGeometry, const math::box2 cullingRect, bool& bClipToBounds, bool& bIntersectClipBounds);

        bool hasAnyFocus() const;

        bool hasMouseCapture() const;

        Visibility getVisibility() const { return m_visibilityAttribute.Get(); }

        void setVisibility(Attribute<Visibility> inVisibility) { m_visibilityAttribute = inVisibility; }

        WidgetClipping getClipping() const { return m_widgetClipping; }

        WidgetGeometry findChildGeometry(const WidgetGeometry& myGeometry, std::shared_ptr<Widget> widgetToFind) const;

        bool findChildGeometries(const WidgetGeometry& myGeometry, const std::set<std::shared_ptr<Widget>>& widgetsToFind, std::unordered_map<std::shared_ptr<Widget>, ArrangedWidget>& outResult) const;

        void findChildGeometries_helper(const WidgetGeometry& myGeometry, const std::set<std::shared_ptr<Widget>>& widgetsToFind, std::unordered_map<std::shared_ptr<Widget>, ArrangedWidget>& outResult) const;

        void setDebugInfo(const GuGuUtf8Str& typeString, const GuGuUtf8Str& inFile, int32_t onLine);

        GuGuUtf8Str getLocation() const;

        GuGuUtf8Str getType() const;

		bool IsHovered() const;
    protected:
        std::weak_ptr<Widget> m_parentWidget;
        WidgetGeometry m_geometry;
        math::float2 m_fixedSize;
        uint32_t m_layer;
        WidgetClipping m_widgetClipping;
        Attribute<Visibility> m_visibilityAttribute;

        //debug info
        GuGuUtf8Str m_widgetType;
        GuGuUtf8Str m_createLocation;
        int32_t m_line;

		bool m_hovered;
    };
}