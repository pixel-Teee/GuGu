#include <pch.h>

#include "WindowWidget.h"

#include "Slot.h"
#include "CoreStyle.h"
#include "ElementList.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

#include <Window/Window.h>

#include <Renderer/DeviceManager.h>//IRenderPass

#include <Application/Application.h>

#include <Core/UI/CoreStyle.h>
#include <Core/UI/Overlay.h>

namespace GuGu {
	WindowWidget::WindowWidget()
		: m_defaultBrush(CoreStyle::getStyleSet()->getBrush("CheckerBoard"))
	{
		m_windowType = WindowType::Normal;
	}
	WindowWidget::~WindowWidget()
	{
	}
	void WindowWidget::init(const BuilderArguments& arguments)
	{
		m_windowType = arguments.mType;
		m_bFocusWhenFirstShown = arguments.mFocusWhenFirstShown;
		//m_childWidget = arguments.mContent;
		//m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;

		m_bIsPopupWindow = arguments.mIsPopupWindow;

		m_screenPosition = arguments.mScreenPosition;

		m_userResizeBorder = arguments.muserResizeBorder;

		m_titleBarSize = 24.0f;//todo:修复这个
		//set size
		resize(arguments.mClientSize);

		m_sizingRule = arguments.msizingRule;

		if (arguments.mContent != nullptr)
		{
			m_childWidget = arguments.mContent;
			m_childWidget->m_parentWidget = shared_from_this();
			if (m_childWidget->m_childWidget != nullptr)
			{
				m_childWidget->m_childWidget->setParentWidget(shared_from_this());
			}	
		}
		else
		{
			//this->m_childWidget->setChildWidget(
			//	WIDGET_ASSIGN_NEW(Overlay, m_windowOverlay)
			//	//pop-up layer
			//	+ Overlay::Slot()
			//	(
			//		WIDGET_ASSIGN_NEW(PopupLayer, m_popupLayer, std::static_pointer_cast<WindowWidget>(shared_from_this()))
			//	)
			//);

			//todo:add popup layer widget
		}
	}
	uint32_t WindowWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		if (m_windowType != WindowType::Normal)
		{
			ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_defaultBrush, layer); //background
		}

		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
	}
	GuGu::math::float2 WindowWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
			}
		}		
		return math::float2(0, 0);
	}
	void WindowWidget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one
		//assert(slotNumber == 1);

		if (slotNumber)
		{
			//Padding slotPadding = getSlot(0)->getPadding();
			//
			////horizontal
			//float totalMarginHorizontal = slotPadding.left + slotPadding.right;
			//float marginPreHorizontal = slotPadding.left;
			//float marginPostHorizontal = slotPadding.right;
			//
			//HorizontalAlignment alignment = getSlot(0)->getHorizontalAlignment();
			//
			//float offsetHorizontal = 0;
			//float sizeHorizontal = 0;
			//
			//if (alignment == HorizontalAlignment::Stretch)
			//{
			//	offsetHorizontal = marginPreHorizontal;
			//	sizeHorizontal = std::max((allocatedGeometry.getLocalSize().x - totalMarginHorizontal) * 1.0, 0.0);//todo:multiple scale
			//}
			//else
			//{
			//	float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().x;
			//
			//	//child actual size
			//	const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().x - totalMarginHorizontal), 0.0f);
			//
			//	if (alignment == HorizontalAlignment::Left)
			//	{
			//		offsetHorizontal = marginPreHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//	else if (alignment == HorizontalAlignment::Center)
			//	{
			//		offsetHorizontal = (allocatedGeometry.getLocalSize().x - childSize) / 2.0f + marginPreHorizontal - marginPostHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//	else if (alignment == HorizontalAlignment::Right)
			//	{
			//		offsetHorizontal = allocatedGeometry.getLocalSize().x - childSize - marginPostHorizontal;
			//		sizeHorizontal = childSize;
			//	}
			//}
			//
			////vertical
			//float totalMarginVertical = slotPadding.top + slotPadding.bottom;
			//float marginPreVertical = slotPadding.top;
			//float marginPostVertical = slotPadding.bottom;
			//
			//VerticalAlignment verticalAlignment = getSlot(0)->getVerticalAlignment();
			//
			//float offsetVertical = 0;
			//float sizeVertical = 0;
			//
			//if (verticalAlignment == VerticalAlignment::Stretch)
			//{
			//	offsetVertical = marginPreVertical;
			//	sizeVertical = std::max((allocatedGeometry.getLocalSize().y - totalMarginVertical) * 1.0, 0.0);
			//}
			//else
			//{
			//	float childFixedSize = getSlot(0)->getChildWidget()->getFixedSize().y;
			//
			//	//child actual size
			//	const float childSize = std::max(std::min(childFixedSize, (float)allocatedGeometry.getLocalSize().y - totalMarginVertical), 0.0f);
			//
			//	if (verticalAlignment == VerticalAlignment::Top)
			//	{
			//		offsetVertical = marginPreVertical;
			//		sizeVertical = childSize;
			//	}
			//	else if (verticalAlignment == VerticalAlignment::Center)
			//	{
			//		offsetVertical = (allocatedGeometry.getLocalSize().y - childSize) / 2.0f + marginPreVertical - marginPostVertical;
			//		sizeVertical = childSize;
			//	}
			//	else if (verticalAlignment == VerticalAlignment::Bottom)
			//	{
			//		offsetVertical = allocatedGeometry.getLocalSize().y - childSize - marginPostVertical;
			//		sizeVertical = childSize;
			//	}
			//}

			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				Padding slotPadding = getSlot(0)->getPadding();
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x, slotPadding);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y, slotPadding);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}		
		}

	}	
	SlotBase* WindowWidget::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t WindowWidget::getSlotsNumber() const
	{
		if (m_childWidget) return 1;
		return 0;
	}
	std::shared_ptr<PopupLayer> WindowWidget::onVisualizePopup(const std::shared_ptr<Widget>& popupContent)
	{
		if (m_windowOverlay)
		{
			return std::make_shared<OverlayPopupLayer>(std::static_pointer_cast<WindowWidget>(shared_from_this()), popupContent, m_windowOverlay);
		}

		return nullptr;
	}
	math::int2 WindowWidget::getViewportSize()
	{
		return math::int2(m_size.x, m_size.y);
	}
	void WindowWidget::setChildWidget(std::shared_ptr<Widget> widget)
	{
		std::shared_ptr<SingleChildSlot> slot = std::make_shared<SingleChildSlot>();
		slot->setChildWidget(widget);
		//slot->setHorizontalAlignment(HorizontalAlignment::Stretch);
		//slot->setVerticalAlignment(VerticalAlignment::Stretch);
		m_childWidget = slot;
	}
	void WindowWidget::assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow)
	{
		m_nativeWindow = nativeWindow;
		if (m_size.x != 0.0 && m_size.y != 0.0f)
		{
			m_nativeWindow->reshapeWindow(m_screenPosition, m_size);
		}
		m_windowType = WindowType::Normal;
		//setCachedScreenPosition(nativeWindow->getWindowScreenSpacePosition());
	}	
	std::shared_ptr<Window> WindowWidget::getNativeWindow()
	{
		std::shared_ptr<Window> nativeWindow = m_nativeWindow;
		return nativeWindow;
	}
	void WindowWidget::setCachedScreenPosition(math::float2 newPosition)
	{
		m_screenPosition = newPosition;
	}
	void WindowWidget::resize(math::float2 newSize)
	{
		if ((m_size.x != newSize.x) || (m_size.y != newSize.y))
		{
			//todo:调整 native window 窗口大小
			m_size = newSize;

			if (m_nativeWindow)
			{
				m_nativeWindow->reshapeWindow(m_screenPosition, newSize);
			}	
		}	
		m_fixedSize = newSize;
	}
	math::float2 WindowWidget::getPositionInScreen() const
	{
		return m_screenPosition;
	}
	WidgetGeometry WindowWidget::getWindowGeometryInScreen() const
	{
		//我们对于布局来缩放孩子，但是我们的像素边界是不改变的
		//WidgetGeometry要求大小在局部空间，但是我们的大小存储在屏幕空间
		//我们需要去转换大小到窗口的局部空间，对于widget geometry
		
		//首先得到一个 local to screen transform
		math::affine2 localToScreen(math::float2x2::diagonal(m_nativeWindow->getDpiFactor()), m_screenPosition);

		WidgetGeometry res = WidgetGeometry::makeRoot(math::inverse(localToScreen).transformVector(m_size), localToScreen);

		return res;
	}
	math::box2 WindowWidget::getClientRectInScreen() const
	{
        if(Application::getApplication()->getGlobalPreRotate() == 90.0f || Application::getApplication()->getGlobalPreRotate() == 270.0f)
        {
            math::float2 newSize = math::float2(m_fixedSize.y, m_fixedSize.x);
            return math::box2(math::float2(m_screenPosition), math::float2(m_screenPosition + newSize));
        }
		return math::box2(math::float2(m_screenPosition), math::float2(m_screenPosition + m_fixedSize));
	}
	math::float2 WindowWidget::getClientSizeInScreen() const
	{
		return getClientRectInScreen().diagonal();//size
	}
	math::affine2 WindowWidget::getLocalToScreenTransform() const
	{
		//diagonal 对角线
		math::affine2 localToScreen(math::float2x2::diagonal(m_nativeWindow->getDpiFactor()), m_screenPosition);
		return localToScreen;
	}
	void WindowWidget::moveWindowTo(math::float2 newPosition)
	{
		if (m_nativeWindow)
		{
			math::float2 speculativeScreenPosition(int32_t(newPosition.x), int32_t(newPosition.y));
			setCachedScreenPosition(speculativeScreenPosition);

			m_nativeWindow->moveWindowTo(math::float2(speculativeScreenPosition.x, speculativeScreenPosition.y));
		}
	}
	bool WindowWidget::isAutoSized() const
	{
		return m_sizingRule == SizingRule::AutoSized;//由内容决定大小
	}
	void WindowWidget::requestDestroyWindow()
	{
		Application::getApplication()->requestDestroyWindow(std::static_pointer_cast<WindowWidget>(shared_from_this()));
	}
	void WindowWidget::destroyNativeWindow()
	{
		if (m_nativeWindow)
		{
			m_nativeWindow->destroy();
		}
	}
	bool WindowWidget::OnIsActivateChanged(const WindowActivateEvent& activateEvent)
	{
		const bool bWasDeactivated = activateEvent.getActivationType() == WindowActivateEvent::Deactivate;
		if (bWasDeactivated)
		{
			m_widgetToFocusOnDeactivate.reset();

			if (supportsKeyboardFocus())
			{
				m_widgetToFocusOnDeactivate = Application::getApplication()->getKeyboardFocusedWidget();
				if (!m_widgetToFocusOnDeactivate.lock())
				{
					m_widgetToFocusOnDeactivate = Application::getApplication()->getFocusedWidget();
				}
			}
		}
		else
		{
			if (activateEvent.getActivationType() == WindowActivateEvent::Activate)
			{
				std::shared_ptr<Widget> lockedWidgetToFocus = m_widgetToFocusOnActivate.lock();
				if (lockedWidgetToFocus)
				{
					std::vector<std::shared_ptr<WindowWidget>> windowWidgets;
					windowWidgets.push_back(std::static_pointer_cast<WindowWidget>(shared_from_this()));

					WidgetPath widgetToFocusPath;
					if (Application::getApplication()->findPathToWidget(windowWidgets, lockedWidgetToFocus, widgetToFocusPath))
					{
						Application::getApplication()->setFocus(lockedWidgetToFocus, widgetToFocusPath);
					}
				}
			}
		}

		return true;
	}
	bool WindowWidget::supportsKeyboardFocus() const
	{
		return true;//这里要区分是否是 tool tip 或者 cursor decorator
	}
	void WindowWidget::setWidgetToFocusOnActivate(std::shared_ptr<Widget> inWidget)
	{
		m_widgetToFocusOnActivate = inWidget;
	}
	bool WindowWidget::isFocusedInitially() const
	{
		return m_bFocusWhenFirstShown;
	}
	void WindowWidget::setParentWindow(const std::shared_ptr<WindowWidget>& inParentWindow)
	{
		m_parentWindow = inParentWindow;
	}
	std::weak_ptr<WindowWidget> WindowWidget::getParentWindow() const
	{
		return m_parentWindow;
	}
	WindowZone::Type WindowWidget::getCurrentWindowZone(math::float2 localMousePosition)
	{
		const float windowDpiScale = m_nativeWindow->getDpiFactor();

		const Padding dpiScaledResizeBorder = m_userResizeBorder * windowDpiScale;

		if (localMousePosition.x >= 0 && localMousePosition.x < m_size.x &&
			localMousePosition.y >= 0 && localMousePosition.y < m_size.y)
		{
			int32_t row = 1;
			int32_t col = 1;

			if (m_sizingRule == SizingRule::UserSize)
			{
				if (localMousePosition.x < (dpiScaledResizeBorder.left + 5))
				{
					col = 0;
				}
				else if (localMousePosition.x >= m_size.x - (dpiScaledResizeBorder.right + 5))
				{
					col = 2;
				}

				if (localMousePosition.y < (dpiScaledResizeBorder.top + 5))
				{
					row = 0;
				}
				else if (localMousePosition.y >= m_size.y - (dpiScaledResizeBorder.bottom + 5))
				{
					row = 2;
				}

				bool bInBorder = localMousePosition.x < dpiScaledResizeBorder.left ||
					localMousePosition.x >= m_size.x - dpiScaledResizeBorder.right ||
					localMousePosition.y < dpiScaledResizeBorder.top ||
					localMousePosition.y >= m_size.y - dpiScaledResizeBorder.bottom;

				if (!bInBorder)
				{
					row = 1;
					col = 1;
				}
			}
			static const WindowZone::Type typeZones[3][3] =
			{
				{WindowZone::TopLeftBorder,			WindowZone::TopBorder,		WindowZone::TopRightBorder},
				{WindowZone::LeftBorder,			WindowZone::ClientArea,		WindowZone::RightBorder},
				{WindowZone::BottomLeftBorder,		WindowZone::BottomBorder,	WindowZone::BottomRightBorder},
			};

			WindowZone::Type inZone = typeZones[row][col];

			if (inZone == WindowZone::ClientArea)
			{
				std::shared_ptr<Widget> collisionWidget = Application::getApplication()->locateWidgetInWindow(m_nativeWindow, Application::getApplication()->getCursorPos());
				if (collisionWidget)
				{
					const WindowZone::Type zoneOverride = collisionWidget->getWindowZoneOverride();
					if (zoneOverride != WindowZone::Unspecified)
					{
						inZone = zoneOverride;
					}
					else if (collisionWidget == shared_from_this())
					{
						if ((localMousePosition.y - dpiScaledResizeBorder.top) < m_titleBarSize * windowDpiScale)
						{
							inZone = WindowZone::TitleBar;
						}
					}
				}

				m_windowZone = inZone;
			}
			//todo:判断菜单是否可见
			else
			{
				m_windowZone = inZone;
			}
		}
		else
		{
			m_windowZone = WindowZone::NontInWindow;
		}
		return m_windowZone;
		
	}
	bool WindowWidget::isRegularWindow() const
	{
		return m_windowType != WindowType::ToolTip && !m_bIsPopupWindow;
	}
	void WindowWidget::hideWindow()
	{
		m_nativeWindow->hide();
	}
	void WindowWidget::setContent(const std::shared_ptr<Widget>& inWidget)
	{
		m_childWidget->setChildWidget(inWidget);
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}
	std::shared_ptr<WindowWidget> WindowWidget::makeCursorDecorator()
	{
		std::shared_ptr<WindowWidget> newWindow = WIDGET_NEW(WindowWidget)
			.Type(WindowType::CursorDecorator)
			.IsPopupWindow(true)
			.sizingRule(SizingRule::AutoSized)
			.FocusWhenFirstShown(false);
		return newWindow;
	}

	void WindowWidget::addChildWindow(const std::shared_ptr<WindowWidget>& inChildWindow)
	{
		std::shared_ptr<WindowWidget> previousParent = inChildWindow->m_parentWindow.lock();
		if (previousParent)
		{
			//todo:实现这里
		}

		inChildWindow->m_parentWindow = std::static_pointer_cast<WindowWidget>(shared_from_this());
		
		Application::getApplication()->arrangeWindowToFrontVirtual(m_childWindows, inChildWindow);
	}

	std::vector<std::shared_ptr<WindowWidget>>& WindowWidget::getChildWindows()
	{
		return m_childWindows;
	}

	OverlayPopupLayer::OverlayPopupLayer(const std::shared_ptr<WindowWidget>& initHostWindow, const std::shared_ptr<Widget>& initPopupContent, std::shared_ptr<Overlay> initOverlay)
		: PopupLayer(initHostWindow, initPopupContent)
		, m_hostWindow(initHostWindow)
		, m_overlay(initOverlay)
	{
		m_overlay->addSlot()
		(
			initPopupContent
		);
	}
	void OverlayPopupLayer::remove()
	{
		m_overlay->removeSlot(getContent());
	}
	math::box2 OverlayPopupLayer::getAbsoluteClientRect()
	{
		return m_hostWindow->getClientRectInScreen();
	}
}