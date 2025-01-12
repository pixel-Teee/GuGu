#include <pch.h>

#include "Application.h"

#include <Core/Timer.h>
#include <Core/UI/Events.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/WindowWidget.h>
#include <Core/UI/UIRenderPass.h>
#include <Core/UI/ArrangedWidget.h>
#include <Core/UI/ArrangedWidgetArray.h>
#include <Core/UI/StyleSetCenter.h>
#include <Core/UI/ToolTip.h>
#include <Window/Window.h>
#include <Renderer/Demo.h>
#include <Renderer/Renderer.h>
#include <Renderer/VertexBuffer.h>

#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/TransformComponent.h>

namespace GuGu{
	WindowActivateEvent::ActivationType translationWindowActivationMessage(const WindowActivation activationType)
	{
		WindowActivateEvent::ActivationType result = WindowActivateEvent::Activate;

		switch (activationType)
		{
		case WindowActivation::Activate:
			result = WindowActivateEvent::Activate;
			break;
		case WindowActivation::ActivateByMouse:
			result = WindowActivateEvent::ActivateByMouse;
			break;
		case WindowActivation::Deactivate:
			result = WindowActivateEvent::Deactivate;
			break;
		default:
			break;
		}

		return result;
	}

    Application::Application()
    {
        m_alreadyExit = false;
        m_focused = true;
        m_timer = CreateTimerFactory();
        m_lastCursorPos = math::float2(0.0f, 0.0f);

		std::shared_ptr<StyleSet> coreStyleSet = std::make_shared<CoreStyle>();
		StyleSetCenter::RegisterStyleSet("CoreStyleSet", coreStyleSet);//register style
    }
	Application::~Application()
	{
	}
	void Application::init(std::shared_ptr<WindowWidget> inWindowWidget)
	{
		//create level
		m_currentLevel = std::make_shared<Level>();
		std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
		std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>();
		transformComponent->SetRotation(math::rotationQuat(math::double3(math::radians(60.0), math::radians(135.0), math::radians(40.0))));
		gameObject->addComponent(transformComponent);
		m_currentLevel->addGameObject(gameObject);
	}
    void Application::Run()
    {
        while (!m_alreadyExit)
        {
            //todo:add update
            pumpMessage();

            m_timer->Tick();
            calculateFrameStats();

            if(m_focused)
            {
				//1.更新关卡和里面的game object
				m_currentLevel->Update(m_timer->GetDeltaTime());
                UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
                Demo* demoPass = m_renderer->getDemoPass();
				//demoPass->setLevel()
				demoPass->renderLevel(m_currentLevel);
                VertexBuffer* vertexBuffer = m_renderer->getVertexBufferPass();
                uiRenderPass->setRenderTarget(demoPass->getRenderTarget());
                m_renderer->onRender();
            }
        }

        m_renderer->onDestroy();
    }
    void Application::pumpMessage()
    {

    }
    void Application::setExit(bool value)
    {
        m_alreadyExit = value;
    }
	void Application::setFocused(bool value) {
		m_focused = value;
	}
	bool Application::getFocused() {
		return m_focused;
	}
    std::shared_ptr<Renderer> Application::getRenderer() {
        return m_renderer;
    }
    std::shared_ptr<Timer> Application::getTimer() {
        return m_timer;
    }
    void Application::miniMizeWindow(std::shared_ptr<WindowWidget> windowWidget)
    {
    }
    void Application::calculateFrameStats()
    {
        static int32_t frameCnt = 0;
        static float timeElapsed = 0.0f;

        ++frameCnt;

        if (m_timer->GetTotalTime() - timeElapsed >= 1.0f)
        {
			fps = (float)frameCnt;
			mfps = 1000.0f / fps;

            frameCnt = 0;
            timeElapsed += 1.0f;
        }
    }

    std::shared_ptr<Window> Application::getWindow(uint32_t index)
    {
        //check index
        return nullptr;
    }

	const std::vector<std::shared_ptr<GuGu::WindowWidget>>& Application::getWindows() const
	{
		return m_windowWidgets;
	}

	float Application::getFps() const
    {
        return fps;
    }

    float Application::getmFps() const
    {
        return mfps;
    }

	Key translateMouseButtonToKey(const MouseButtons::Type button)
	{
		Key key;
		switch (button)
		{
		case MouseButtons::Left:
			key = Keys::LeftMouseButton;
			return key;
		case MouseButtons::Right:
			key = Keys::RightMouseButton;
			return key;
		case MouseButtons::Middle:
			key = Keys::MiddleMouseButton;
			return key;
		case MouseButtons::Thumb01:
			key = Keys::ThumbMouseButton;
			return key;
		case MouseButtons::Thumb02:
			key = Keys::ThumbMouseButton2;
			return key;
		}

		return key;
	}

	bool Application::onMouseDown(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos)
	{
		//process mouse button down

		//find window widget
		//find window widget
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

        //transfer cursorPos to local space
        math::float2 translatedCursorPos = translateCursorPos(cursorPos, windowWidget);


        PointerEvent mouseEvent(
            translatedCursorPos,
            m_lastCursorPos,
			translateMouseButtonToKey(mouseButton)
        );
        m_lastCursorPos = translatedCursorPos;

        return processMouseButtonDownEvent(window, mouseEvent);
	}

    bool Application::onMouseUp(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos)
    {
		//find window widget
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

        math::float2 translatedCursorPos = translateCursorPos(cursorPos, windowWidget);

		PointerEvent mouseEvent(
            translatedCursorPos,
			m_lastCursorPos,
			translateMouseButtonToKey(mouseButton)
		);
		m_lastCursorPos = translatedCursorPos;

        return processMouseButtonUpEvent(window, mouseEvent);
    }

    bool Application::onMouseMove(const std::shared_ptr<Window>& window, math::float2 cursorPos)
    {
		//find window widget
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		math::float2 translatedCursorPos = translateCursorPos(cursorPos, windowWidget);

		PointerEvent mouseEvent(
			translatedCursorPos,
			m_lastCursorPos
		);
		m_lastCursorPos = translatedCursorPos;

		return processMouseMoveEvent(window, mouseEvent);
    }

	bool Application::onMouseDoubleClick(const std::shared_ptr<Window>& window, MouseButtons::Type mouseButton, math::float2 cursorPos)
	{
		//find window widget
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		math::float2 translatedCursorPos = translateCursorPos(cursorPos, windowWidget);

		PointerEvent mouseEvent(
			translatedCursorPos,
			m_lastCursorPos,
			translateMouseButtonToKey(mouseButton)
		);

		return processMouseButtonDoubleClickEvent(window, mouseEvent);
	}

	bool Application::onKeyChar(const GuGuUtf8Str Character)
	{
		CharacterEvent characterEvent(Character);
		return processKeyCharEvent(characterEvent);
	}

	bool Application::onKeyDown(const int32_t keyCode, const uint32_t characterCode)
	{
		const Key key = InputKeyManager::Get().getKeyFromCodes(keyCode, characterCode);

		//字符串名，字符码，虚拟键码
		KeyEvent keyEvent(key, getModifierKeys(),characterCode, keyCode);

		return processKeyDownEvent(keyEvent);
	}

	bool Application::onWindowActivationChanged(const std::shared_ptr<Window>& window, const WindowActivation activationType)
	{
		//find window widget by platform windows
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		WindowActivateEvent::ActivationType translatedActivationType = translationWindowActivationMessage(activationType);
		WindowActivateEvent windowActivateEvent(translatedActivationType, windowWidget);

		//todo:add process window activated event
		return processWindowActivatedEvent(windowActivateEvent);
	}

	void Application::onMovedWindow(const std::shared_ptr<Window>& window, const int32_t x, const int32_t y)
	{
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		if (windowWidget)
		{
			windowWidget->setCachedScreenPosition(math::float2(x, y));
		}
	}

	bool Application::onMouseWheel(const std::shared_ptr<Window>& window, const float delta, const math::float2 cursorPos)
	{
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		math::float2 translatedCursorPos = translateCursorPos(cursorPos, windowWidget);

		PointerEvent mouseWheelEvent(
			cursorPos,
			m_lastCursorPos,
			translateMouseButtonToKey(MouseButtons::Type::Left),
			delta
		);

		return processMouseWheelEvent(window, mouseWheelEvent);
	}

    std::shared_ptr<Widget> Application::getCaptorWidget() const
    {
        if (!m_captorWidgetsPath.isEmpty())
            return m_captorWidgetsPath.getLastWidget().lock();
        return nullptr;
    }

	WindowZone::Type Application::getWindowZoneForPoint(const std::shared_ptr<Window>& window, const int32_t X, const int32_t Y)
	{
		return WindowZone::NontInWindow;
	}

	bool Application::hasAnyFocus(std::shared_ptr<const Widget> inWidget) const
	{
		//if(!m_focusWidgetsPath.empty())
		//	return m_focusWidgetsPath.front().lock() == inWidget;
		//return false;

		if (!m_focusWidgetsPath.isEmpty())
			return m_focusWidgetsPath.getLastWidget().lock() == inWidget;
		return false;
	}

	bool Application::doesWidgetHaveMouseCapture(std::shared_ptr<const Widget> inWidget) const
	{
		//if (!m_captorWidgetsPath.empty())
		//	return m_captorWidgetsPath.front().lock() == inWidget;
		//return false;

		if (!m_captorWidgetsPath.isEmpty())
			return m_captorWidgetsPath.getLastWidget().lock() == inWidget;
		return false;
	}

	void Application::setGlobalPreRotate(float rotation)
    {
        m_globalRotation = rotation;
    }

    float Application::getGlobalPreRotate() const
    {
        return m_globalRotation;
    }

	void Application::processReply(const Reply& reply, const WidgetPath& widgetPath, const PointerEvent* inMouseEvent)
	{
		if (reply.shouldReleaseMouse())
		{
			m_widgetsUnderPointerLastEvent = m_captorWidgetsPath;
			m_captorWidgetsPath.clear();
		}

		//释放之前的焦点路径
		//if (m_captorWidgetsPath.isEmpty() == false)
		//{
		//	m_captorWidgetsPath.clear();
		//}
		
		std::shared_ptr<Widget> requestedMouseCaptor = reply.getMouseCaptor();
		if (requestedMouseCaptor != nullptr)
		{
			//m_captorWidget = mouseCaptor;
			//m_captorWidgetsPath.clear();
			//for (int32_t j = i; j < widgets.size(); ++j)
			//	m_captorWidgetsPath.push_back(widgets[i]);
			m_captorWidgetsPath = widgetPath.pathDownTo(requestedMouseCaptor);
		}

		if (reply.getDetectDragRequest())
		{
			assert(inMouseEvent != nullptr);
			m_dragstates = DragDetectionState(widgetPath.pathDownTo(reply.getDetectDragRequest()), reply.getDetectDragRequestButton(), inMouseEvent->m_screenSpacePosition);
		}

		std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
		if (requestedFocusRecepient) //请求焦点的控件是有效的
		{		
			//for (int32_t j = i; j < widgets.size(); ++j)
			//	m_focusWidgetsPath.push_back(widgets[j]);
			setFocus(requestedFocusRecepient, widgetPath);
		}
	}

	void Application::setFocus(const std::shared_ptr<Widget>& widgetToFocus, const WidgetPath& widgetPath)
	{
		WeakWidgetPath oldFocusedWidgetPath = m_focusWidgetsPath;
		std::shared_ptr<Widget> oldFocusedWidget = oldFocusedWidgetPath.isEmpty() ? std::shared_ptr<Widget>() : oldFocusedWidgetPath.getLastWidget().lock();

		WidgetPath newFocusedWidgetPath;
		std::shared_ptr<Widget> newFocusedWidget;

		if (widgetPath.isValid())
		{
			int32_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t widgetIndex = widgetNumber - 1; widgetIndex >= 0; --widgetIndex)
			{
				 const std::shared_ptr<ArrangedWidget>& widgetToFocus = widgetPath.m_widgets[widgetIndex];

				 if (widgetToFocus->getWidget()->supportsKeyboardFocus())
				 {
					 if (widgetToFocus->getWidget() == oldFocusedWidget)
					 {
						 return; //todo:fix this
					 }
					 newFocusedWidget = widgetToFocus->getWidget();
					 newFocusedWidgetPath = widgetPath.pathDownTo(newFocusedWidget);
					 break;
				 }
			}
		}

		if (oldFocusedWidgetPath.isValid())
		{
			for (int32_t childIndex = 0; childIndex < oldFocusedWidgetPath.m_widgets.size(); ++childIndex)
			{
				std::shared_ptr<Widget> someWidget = oldFocusedWidgetPath.m_widgets[childIndex].lock();
				if (someWidget)
				{
					someWidget->OnFocusChanging(oldFocusedWidgetPath, newFocusedWidgetPath);
				}
			}
		}

		if (newFocusedWidgetPath.isValid())
		{
			int32_t widgetNumber = newFocusedWidgetPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t childIndex = 0; childIndex < widgetNumber; ++childIndex)
			{
				std::shared_ptr<Widget> someWidget = newFocusedWidgetPath.m_widgets[childIndex]->getWidget();
				if (someWidget)
				{
					someWidget->OnFocusChanging(oldFocusedWidgetPath, newFocusedWidgetPath);
				}
			}
		}

		if (oldFocusedWidget)
		{
			oldFocusedWidget->OnFocusLost();
		}

		if (newFocusedWidget)
		{
			const ArrangedWidget& widgetToFocus = newFocusedWidgetPath.m_widgets.back();
			Reply reply = newFocusedWidget->OnFocusReceived(widgetToFocus.getWidgetGeometry());
			if (reply.isEventHandled())
			{
				processReply(reply, widgetPath);
			}
		}

		m_focusWidgetsPath = newFocusedWidgetPath;

		////记录旧的焦点路径
		//WidgetPath oldFocusWidgetsPath;
		//m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);
		//
		//std::shared_ptr<Widget> oldFocus;
		//if (oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() != 0)
		//{
		//	oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		//}
		//
		////set user focus(这里是设置用户焦点的代码逻辑)
		////设置新的焦点控件
		//size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
		//for (int32_t i = widgetNumber - 1; i >= 0; --i)
		//{
		//	std::shared_ptr<Widget> widget = widgetPath.m_widgets[i]->getWidget();
		//	if (widget->supportsKeyboardFocus())
		//	{
		//		WidgetPath focusedWidgetPath;
		//		focusedWidgetPath = widgetPath.pathDownTo(widget);
		//
		//		if (widget == oldFocus) //focus 没变化，不调用onFocusLost和onFocusReceived等函数
		//		{
		//			return;
		//		}
		//
		//		int32_t focusedWidgetNumber = focusedWidgetPath.m_widgets.getArrangedWidgetsNumber();
		//		for (int32_t j = focusedWidgetNumber - 1; j >= 0; --j)
		//		{
		//			std::shared_ptr<Widget> widget = focusedWidgetPath.m_widgets[j]->getWidget();
		//			m_focusWidgetsPath.clear();
		//			m_focusWidgetsPath = focusedWidgetPath.pathDownTo(widget);
		//			break;
		//		}
		//
		//		if (oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() != 0)
		//		{
		//			//std::shared_ptr<Widget> oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		//			oldFocus->OnFocusLost();
		//		}
		//
		//		if (focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() != 0)
		//		{
		//			std::shared_ptr<Widget> newFocusWidget = focusedWidgetPath.m_widgets[focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		//			newFocusWidget->OnFocusReceived(newFocusWidget->getWidgetGeometry());
		//		}
		//
		//		//std::shared_ptr<Widget> oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		//		//
		//		//oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget()->OnFocusLost();
		//		//
		//		//std::shared_ptr<Widget> newFocusWidget = focusedWidgetPath.m_widgets[focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		//		//newFocusWidget->OnFocusReceived(newFocusWidget->getWidgetGeometry());
		//
		//		break;
		//	}
		//}
	}

	std::shared_ptr<Widget> Application::getKeyboardFocusedWidget() const
	{
		if (m_focusWidgetsPath.isEmpty())
			return nullptr;
		return m_focusWidgetsPath.getLastWidget().lock();
	}

	std::shared_ptr<Widget> Application::getFocusedWidget() const
	{
		return m_focusWidgetsPath.isValid() ? m_focusWidgetsPath.getLastWidget().lock() : nullptr;
	}

	bool Application::findPathToWidget(const std::vector<std::shared_ptr<WindowWidget>>& windowsToSearch, std::shared_ptr<Widget> inWidget, WidgetPath& outWidgetPath, Visibility visibilityFilter)
	{
		//迭代我们的顶层窗口
		bool bFoundWidget = false;

		for (int32_t windowIndex = 0; !bFoundWidget && windowIndex < windowsToSearch.size(); ++windowIndex)
		{
			std::shared_ptr<WindowWidget> curWindow = windowsToSearch[windowIndex];

			ArrangedWidgetArray justWindow(visibilityFilter);
			{
				//注意这个geometry追加了窗口左上角的绝对位置
				justWindow.pushWidget(curWindow->getWindowGeometryInScreen(), curWindow);
			}

			WidgetPath pathToWidget(curWindow, justWindow);

			//是否能获取得到一条路径从 curWindow 到 inWidget
			if ((curWindow == inWidget) || pathToWidget.extendPathTo(inWidget, visibilityFilter))
			{
				outWidgetPath = pathToWidget;
				bFoundWidget = true;
			}
		}

		return bFoundWidget;
	}

	bool Application::generatePathToWidgetUnchecked(std::shared_ptr<Widget> inWidget, WidgetPath& outWidgetPath, Visibility visibilityFilter) const
	{
// 		UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
// 		std::shared_ptr<WindowWidget> windowWidget = uiRenderPass->getWindowWidget();//这里要修复
// 		std::vector<std::shared_ptr<WindowWidget>> widgets;
// 		widgets.push_back(windowWidget);
		if (!findPathToWidget(m_windowWidgets, inWidget, outWidgetPath, visibilityFilter))
		{
			return findPathToWidget(m_windowWidgets, inWidget, outWidgetPath, visibilityFilter);
		}

		return true;
	}

	void Application::makeWindow(std::shared_ptr<WindowWidget> windowWidget)
	{
		//由子类来实现
	}

	void Application::showWindow(std::shared_ptr<WindowWidget> windowWidget)
	{
		//由子类来实现
	}

	std::vector<std::shared_ptr<WindowWidget>>& Application::getWidowWidgets()
	{
		return m_windowWidgets;
	}

	void Application::OnResizeChanged(std::shared_ptr<Window> inWindow)
	{
		m_renderer->requestResize(inWindow);
	}

	void Application::requestDestroyWindow(std::shared_ptr<WindowWidget> windowToDestroy)
	{
		//todo:call menu stack OnWindowDestroyed

		//真正的销毁
		m_renderer->onWindowDestroyed(windowToDestroy);

		windowToDestroy->destroyNativeWindow();

		auto it = std::find(m_windowWidgets.begin(), m_windowWidgets.end(), windowToDestroy);
		if (it != m_windowWidgets.end())
		{
			m_windowWidgets.erase(it);
		}
		//没有窗口，则要退出应用程序
		if (m_windowWidgets.size() == 0)
		{
			setExit(true);
		}
	}

	std::shared_ptr<IMenu> Application::pushMenu(const std::shared_ptr<Widget>& inParentWidget, const WidgetPath& inOwnerPath, const std::shared_ptr<Widget>& inContent, const math::float2& summonLocation, const bool bFocusImmediately, const math::float2& summonLocationSize, std::optional<PopupMethod> method, const bool bIsCollapsedByParent)
	{
		//调用者提供了一条完整的路径
		if (inOwnerPath.isValid())
		{
			return m_menuStack.push(inOwnerPath, inContent, summonLocation, bFocusImmediately, summonLocationSize, method, bIsCollapsedByParent);
		}

		//如果调用者没有提供一个有效的路径，我们需要生成一条
		WidgetPath widgetPath;
		if (generatePathToWidgetUnchecked(inParentWidget, widgetPath))
		{
			return m_menuStack.push(widgetPath, inContent, summonLocation, bFocusImmediately, summonLocationSize, method, bIsCollapsedByParent);
		}

		return std::shared_ptr<IMenu>();
	}

	math::float2 Application::calculatePopupWindowPosition(const math::box2& inAnchor, const math::float2& inSize, bool bAutoAdjustForDPIScale, const math::float2& inProposedPlacement, const Orientation orientation) const
	{
		math::float2 calculatedPopupWindowPosition(0, 0);

		float dpiScale = 1.0f;

		if (bAutoAdjustForDPIScale)
		{
			//dpiScale = PlatformMisc::getDpi
			//todo:实现这里
		}

		math::float2 adjustedSize = inSize * dpiScale;

		math::box2 anchorRect;
		anchorRect.m_mins.x = inAnchor.m_mins.x;//left
		anchorRect.m_mins.y = inAnchor.m_mins.y;//top
		anchorRect.m_maxs.x = inAnchor.m_maxs.x;//right
		anchorRect.m_maxs.y = inAnchor.m_maxs.y;//bottom

		math::box2 workAreaFinderRect(anchorRect);
		workAreaFinderRect.m_mins.x = anchorRect.m_mins.x + 1;
		workAreaFinderRect.m_mins.y = anchorRect.m_mins.y + 1;
		const math::box2 platformWorkArea = getWorkArea(workAreaFinderRect);

		//todo:fix this
		return math::float2(anchorRect.m_mins.x, anchorRect.m_mins.y);
	}

	math::box2 Application::getWorkArea(const math::box2& currentWindow) const
	{
		//return math::box2(0);
		math::box2 workArea;
		workArea.m_mins = math::float2(0, 0);
		workArea.m_maxs = math::float2(0, 0);
		return workArea;
	}

	std::shared_ptr<Level> Application::getCurrentLevel()
	{
		return m_currentLevel;
	}

	const std::shared_ptr<Level> Application::getCurrentLevel() const
	{
		return m_currentLevel;
	}

	void Application::closeTooltip()
	{
		m_activeTooltipInfo.reset();

		std::shared_ptr<WindowWidget> tooltipWindow = m_toolTipWindowPtr.lock();
		if (tooltipWindow)
		{
			tooltipWindow->hideWindow();
		}
	}

	void Application::showTooltip(const std::shared_ptr<IToolTip>& inTooltip, const math::float2& inLocation)
	{
		closeTooltip();

		m_activeTooltipInfo.m_toolTip = inTooltip;

		std::shared_ptr<WindowWidget> tooltipWindow = getOrCreateTooltipWindow();
		tooltipWindow->setContent(
			inTooltip->asWidget()
		);//todo:修复这里

		//todo:prepass
		tooltipWindow->prepass(tooltipWindow->getNativeWindow()->getDpiFactor());

		m_activeTooltipInfo.m_desiredLocation = inLocation;
		tooltipWindow->moveWindowTo(m_activeTooltipInfo.m_desiredLocation);
		showWindow(tooltipWindow);
	}

	std::shared_ptr<WindowWidget> Application::getOrCreateTooltipWindow()
	{
		if (m_toolTipWindowPtr.lock())
		{
			return m_toolTipWindowPtr.lock();
		}

		std::shared_ptr<WindowWidget> newTooltipWindow = WIDGET_NEW(WindowWidget)
			.ClientSize(math::float2(512.0f, 256.0f))
			.ScreenPosition(0.0)
			.sizingRule(SizingRule::AutoSized)
			.IsPopupWindow(true);
		m_toolTipWindowPtr = newTooltipWindow;
		makeWindow(newTooltipWindow);	
		return newTooltipWindow;
	}

	void Application::resetTooltipWindow()
	{
		if (m_toolTipWindowPtr.lock())
		{
			m_toolTipWindowPtr.lock()->requestDestroyWindow();
		}	
		m_toolTipWindowPtr.reset();
	}

	bool Application::processMouseButtonDownEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		if (!m_captorWidgetsPath.isEmpty())
		{
			//std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
			WidgetPath captorWidgetsPath;
			m_captorWidgetsPath.toWidgetPath(captorWidgetsPath);
			int32_t widgetNumber = captorWidgetsPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t i = widgetNumber - 1; i >= 0; --i)
			{
				std::shared_ptr<Widget> widget = captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidget();
				if (widget != nullptr)
				{
					Reply reply = widget->OnMouseButtonDown(captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidgetGeometry(), mouseEvent);
					processReply(reply, captorWidgetsPath);
					//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					//if (mouseCaptor != nullptr)
					//{
					//	//m_captorWidget = mouseCaptor;
					//	m_captorWidgetsPath.clear();
					//	for (int32_t j = i; j < captorWidgetsPath.size(); ++j)
					//		m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					//}
					//if (reply.shouldReleaseMouse())
					//{
					//	m_captorWidgetsPath.clear();
					//}
					if (reply.isEventHandled())
						break;
				}
			}
		}
		else
        {
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent.m_screenSpacePosition);

			std::vector<std::shared_ptr<Widget>> widgets;
			std::shared_ptr<Widget> currentWidget = collisionWidget;
			while (currentWidget)
			{
				widgets.push_back(currentWidget);
				currentWidget = currentWidget->getParentWidget();
			}
			std::reverse(widgets.begin(), widgets.end()); //构造widget path
			WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
			windowOffsetGeometry.mAbsolutePosition = window->getWindowScreenSpacePosition();
			WidgetPath widgetPath(widgets, windowOffsetGeometry);

			const std::shared_ptr<Widget> previousFocusedWidget = getKeyboardFocusedWidget();

			//从碰撞到的widget开始派发事件
			size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			Reply reply = Reply::Unhandled();
			for (int32_t i = widgetNumber - 1; i >= 0; --i) //冒泡策略
			{
				reply = widgetPath.m_widgets[i]->getWidget()->OnMouseButtonDown(widgetPath.m_widgets[i]->getWidgetGeometry(), mouseEvent);

				//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				//if (mouseCaptor != nullptr)
				//{
				//	//m_captorWidget = mouseCaptor;
				//	m_captorWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_captorWidgetsPath.push_back(widgets[i]);
				//}
				//if (reply.shouldReleaseMouse())
				//{
				//	m_captorWidgetsPath.clear();
				//}
				//std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				//if (requestedFocusRecepient)
				//{
				//	m_focusWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_focusWidgetsPath.push_back(widgets[j]);
				//}

				processReply(reply, widgetPath);

				if (reply.isEventHandled())
					break;
			}

			const bool bFocusedChangedByEventyHandler = previousFocusedWidget != getKeyboardFocusedWidget();
			
			//焦点控件和之前不一样，或者没有焦点控件被请求
			if (!bFocusedChangedByEventyHandler && !reply.getFocusRecepient())
			{
				for (int32_t i = widgetNumber - 1; i >= 0; --i) //bubble policy
				{
					std::shared_ptr<Widget> widget = widgetPath.m_widgets[i]->getWidget();
					if (widget->supportsKeyboardFocus())
					{
						WidgetPath newFocusedWidgetPath;
						widgetPath.pathDownTo(widget);
						setFocus(widget, widgetPath);
						break;
					}
				}
			}
        }
        
        return true;
    }

    bool Application::processMouseButtonUpEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		if (!m_captorWidgetsPath.isEmpty())
		{
			//std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
			WidgetPath captorWidgetsPath;
			m_captorWidgetsPath.toWidgetPath(captorWidgetsPath);
			int32_t widgetNumber = captorWidgetsPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t i = widgetNumber - 1; i >= 0; --i)
			{
				std::shared_ptr<Widget> widget = captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidget();
				if (widget != nullptr)
				{
					Reply reply = widget->OnMouseButtonUp(captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidgetGeometry(), mouseEvent);
					processReply(reply, captorWidgetsPath);
					//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					//if (mouseCaptor != nullptr)
					//{
					//	//m_captorWidget = mouseCaptor;
					//	m_captorWidgetsPath.clear();
					//	for (int32_t j = i; j < captorWidgetsPath.size(); ++j)
					//		m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					//}
					//if (reply.shouldReleaseMouse())
					//{
					//	m_captorWidgetsPath.clear();
					//}
					if (reply.isEventHandled())
						break;
				}
			}
		}
		else
		{
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent.m_screenSpacePosition);

			std::vector<std::shared_ptr<Widget>> widgets;
			std::shared_ptr<Widget> currentWidget = collisionWidget;
			while (currentWidget)
			{
				widgets.push_back(currentWidget);
				currentWidget = currentWidget->getParentWidget();
			}
			std::reverse(widgets.begin(), widgets.end()); //构造widget path
			WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
			windowOffsetGeometry.mAbsolutePosition = window->getWindowScreenSpacePosition();
			WidgetPath widgetPath(widgets, windowOffsetGeometry);

			//记录旧的焦点路径
			WidgetPath oldFocusWidgetsPath;
			m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);

			//从碰撞到的widget开始派发事件
			size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			Reply reply = Reply::Unhandled();
			for (int32_t i = widgetNumber - 1; i >= 0; --i) //bubble policy
			{
				reply = widgetPath.m_widgets[i]->getWidget()->OnMouseButtonUp(widgetPath.m_widgets[i]->getWidgetGeometry(), mouseEvent);

				//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				//if (mouseCaptor != nullptr)
				//{
				//	//m_captorWidget = mouseCaptor;
				//	m_captorWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_captorWidgetsPath.push_back(widgets[i]);
				//}
				//if (reply.shouldReleaseMouse())
				//{
				//	m_captorWidgetsPath.clear();
				//}
				//std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				//if (requestedFocusRecepient)
				//{
				//	m_focusWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_focusWidgetsPath.push_back(widgets[j]);
				//}

				processReply(reply, widgetPath);
				if (reply.isEventHandled())
					break;
			}
		}
		

		//if (collisionWidget)
		//{
		//    collisionWidget->OnMouseButtonDown(collisionWidget->getWidgetGeometry(), mouseEvent);
		//	//std::shared_ptr<ImageWidget> imageWidget = std::dynamic_pointer_cast<ImageWidget>(collisionWidget);
		//	//if (imageWidget)
		//	//{
		//	//    GuGu_LOGD("%s", u8"image widget");
		//	//}
		//}
		//GuGu_LOGD("(%f %f)", cursorPosition.x, cursorPosition.y);
		return true;
    }

    bool Application::processMouseMoveEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		updateToolTip(m_menuStack, true);

		//------当前获取得到的控件路径------
		std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent.m_screenSpacePosition);

		std::vector<std::shared_ptr<Widget>> widgets;
		std::shared_ptr<Widget> currentWidget = collisionWidget;
		while (currentWidget)
		{
			widgets.push_back(currentWidget);
			currentWidget = currentWidget->getParentWidget();
		}
		std::reverse(widgets.begin(), widgets.end()); //构造widget path
		WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
		windowOffsetGeometry.mAbsolutePosition = window->getWindowScreenSpacePosition();
		WidgetPath widgetPath(widgets, windowOffsetGeometry);
		//------当前获取得到的控件路径------

		//------上一次事件处理的控件路径------
		WeakWidgetPath lastWidgetsUnderPointer = m_widgetsUnderPointerLastEvent;
		//------上一次事件处理的控件路径------

		int32_t preViousUnderCusorWidgetNumber = m_widgetsUnderPointerLastEvent.m_widgets.size();
		for (int32_t widgetIndex = preViousUnderCusorWidgetNumber - 1; widgetIndex >= 0; --widgetIndex)
		{
			const std::shared_ptr<Widget>& someWidgetPreviouslyUnderCursor = lastWidgetsUnderPointer.m_widgets[widgetIndex].lock();
			if (someWidgetPreviouslyUnderCursor)
			{
				if (!widgetPath.containsWidget(someWidgetPreviouslyUnderCursor.get())) //不再交互
				{
					someWidgetPreviouslyUnderCursor->OnMouseLeave(mouseEvent);
				}
			}
		}

		if (!m_captorWidgetsPath.isEmpty())
		{
			//std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
			WidgetPath captorWidgetsPath;
			m_captorWidgetsPath.toWidgetPath(captorWidgetsPath);
			int32_t widgetNumber = captorWidgetsPath.m_widgets.getArrangedWidgetsNumber();

			int32_t widgetUnderCusorWidgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t i = widgetUnderCusorWidgetNumber - 1; i >= 0; --i) //处理鼠标移入一个控件的事件
			{
				std::shared_ptr<Widget> widgetUnderCursor = widgetPath.m_widgets.getArrangedWidget(i)->getWidget();
				if (!lastWidgetsUnderPointer.containsWidget(widgetUnderCursor.get()))
				{
					if (captorWidgetsPath.containsWidget(widgetUnderCursor.get()))
					{
						widgetPath.m_widgets.getArrangedWidget(i)->getWidget()->OnMouseEnter(widgetUnderCursor->getWidgetGeometry(), mouseEvent);
						//processReply(reply, widgetPath);
					}	
				}
			}

			for (int32_t i = widgetNumber - 1; i >= 0; --i)
			{
				std::shared_ptr<Widget> widget = captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidget();
				if (widget != nullptr)
				{
					Reply reply = widget->OnMouseMove(captorWidgetsPath.m_widgets.getArrangedWidget(i)->getWidgetGeometry(), mouseEvent);
					processReply(reply, captorWidgetsPath);
					//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					//if (mouseCaptor != nullptr)
					//{
					//	//m_captorWidget = mouseCaptor;
					//	m_captorWidgetsPath.clear();
					//	for (int32_t j = i; j < captorWidgetsPath.size(); ++j)
					//		m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					//}
					//if (reply.shouldReleaseMouse())
					//{
					//	m_captorWidgetsPath.clear();
					//}
					if (reply.isEventHandled())
						break;
				}
			}
		}
		else
		{
			int32_t widgetUnderCusorWidgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t i = widgetUnderCusorWidgetNumber - 1; i >= 0; --i) //处理鼠标移入一个控件的事件
			{
				std::shared_ptr<Widget> widgetUnderCursor = widgetPath.m_widgets.getArrangedWidget(i)->getWidget();
				if (!lastWidgetsUnderPointer.containsWidget(widgetUnderCursor.get()))
				{	
					widgetPath.m_widgets.getArrangedWidget(i)->getWidget()->OnMouseEnter(widgetUnderCursor->getWidgetGeometry(), mouseEvent);
					//processReply(reply, widgetPath);
				}
			}

			//记录旧的焦点路径
			WidgetPath oldFocusWidgetsPath;
			m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);

			//widgets 第一个是 window

			//从碰撞到的widget开始派发事件
			size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
			for (int32_t i = widgetNumber - 1; i >= 0; --i) //bubble policy
			{
				Reply reply = widgetPath.m_widgets[i]->getWidget()->OnMouseMove(widgetPath.m_widgets[i]->getWidgetGeometry(), mouseEvent);

				//std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				//if (mouseCaptor != nullptr)
				//{
				//	//m_captorWidget = mouseCaptor;
				//	m_captorWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_captorWidgetsPath.push_back(widgets[i]);
				//}
				//if (reply.shouldReleaseMouse())
				//{
				//	m_captorWidgetsPath.clear();
				//}
				//std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				//if (requestedFocusRecepient)
				//{
				//	m_focusWidgetsPath.clear();
				//	for (int32_t j = i; j < widgets.size(); ++j)
				//		m_focusWidgetsPath.push_back(widgets[j]);
				//}

				processReply(reply, widgetPath);
				if (reply.isEventHandled())
					break;
			}
		}


		//if (collisionWidget)
		//{
		//    collisionWidget->OnMouseButtonDown(collisionWidget->getWidgetGeometry(), mouseEvent);
		//	//std::shared_ptr<ImageWidget> imageWidget = std::dynamic_pointer_cast<ImageWidget>(collisionWidget);
		//	//if (imageWidget)
		//	//{
		//	//    GuGu_LOGD("%s", u8"image widget");
		//	//}
		//}
		//GuGu_LOGD("(%f %f)", cursorPosition.x, cursorPosition.y);

		//通知鼠标移动完成
		m_widgetsUnderPointerLastEvent = WeakWidgetPath(widgetPath);
		return true;
    }

	bool Application::processKeyCharEvent(const CharacterEvent& inCharacterEvent)
	{
		Reply reply = Reply::Unhandled();

		//focus path
		WidgetPath focusPath;
		m_focusWidgetsPath.toWidgetPath(focusPath);

		int32_t focusWidgetNumber = focusPath.m_widgets.getArrangedWidgetsNumber();
		for (int32_t i = focusWidgetNumber - 1; i >= 0; --i)
		{
			std::shared_ptr<Widget> widget = focusPath.m_widgets.getArrangedWidget(i)->getWidget();
			widget->OnKeyChar(widget->getWidgetGeometry(), inCharacterEvent);
		}

		return true;
	}

	bool Application::processKeyDownEvent(const KeyEvent& inKeyEvent)
	{
		Reply reply = Reply::Unhandled();

		//focus path
		WidgetPath focusPath;
		m_focusWidgetsPath.toWidgetPath(focusPath);

		int32_t focusWidgetNumber = focusPath.m_widgets.getArrangedWidgetsNumber();
		for (int32_t i = focusWidgetNumber - 1; i >= 0; --i)
		{
			std::shared_ptr<Widget> widget = focusPath.m_widgets.getArrangedWidget(i)->getWidget();
			widget->OnKeyDown(widget->getWidgetGeometry(), inKeyEvent);
		}

		return true;
	}

	bool Application::processWindowActivatedEvent(const WindowActivateEvent& activateEvent)
	{
		//todo:继续实现这个函数
		if (activateEvent.getActivationType() != WindowActivateEvent::Deactivate)
		{
			activateEvent.getAffectedWindow()->OnIsActivateChanged(activateEvent);
		}
		else
		{
			activateEvent.getAffectedWindow()->OnIsActivateChanged(activateEvent);
		}

		return false;
	}

	bool Application::processMouseButtonDoubleClickEvent(const std::shared_ptr<Window>&window, const PointerEvent & mouseEvent)
	{
		std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent.m_screenSpacePosition);

		std::vector<std::shared_ptr<Widget>> widgets;
		std::shared_ptr<Widget> currentWidget = collisionWidget;
		while (currentWidget)
		{
			widgets.push_back(currentWidget);
			currentWidget = currentWidget->getParentWidget();
		}
		std::reverse(widgets.begin(), widgets.end()); //构造widget path
		WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
		windowOffsetGeometry.mAbsolutePosition = window->getWindowScreenSpacePosition();
		WidgetPath widgetPath(widgets, windowOffsetGeometry);

		//记录旧的焦点路径
		WidgetPath oldFocusWidgetsPath;
		m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);

		//从碰撞到的widget开始派发事件
		size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
		Reply reply = Reply::Unhandled();
		for (int32_t i = widgetNumber - 1; i >= 0; --i) //bubble policy
		{
			reply = widgetPath.m_widgets[i]->getWidget()->OnMouseButtonDoubleClick(widgetPath.m_widgets[i]->getWidgetGeometry(), mouseEvent);
			processReply(reply, widgetPath);
			if(reply.isEventHandled())
				break;
		}

		return true; 
	}

	bool Application::processMouseWheelEvent(const std::shared_ptr<Window>& window, const PointerEvent& inWheelEvent)
	{
		std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, inWheelEvent.m_screenSpacePosition);

		std::vector<std::shared_ptr<Widget>> widgets;
		std::shared_ptr<Widget> currentWidget = collisionWidget;
		while (currentWidget)
		{
			widgets.push_back(currentWidget);
			currentWidget = currentWidget->getParentWidget();
		}
		std::reverse(widgets.begin(), widgets.end()); //构造widget path
		WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
		windowOffsetGeometry.mAbsolutePosition = window->getWindowScreenSpacePosition();
		WidgetPath widgetPath(widgets, windowOffsetGeometry);

		//记录旧的焦点路径
		WidgetPath oldFocusWidgetsPath;
		m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);

		//从碰撞到的widget开始派发事件
		size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
		Reply reply = Reply::Unhandled();
		for (int32_t i = widgetNumber - 1; i >= 0; --i) //bubble policy
		{
			reply = widgetPath.m_widgets[i]->getWidget()->OnMouseWheel(widgetPath.m_widgets[i]->getWidgetGeometry(), inWheelEvent);
			processReply(reply, widgetPath);
		}

		return true;
	}

    std::shared_ptr<Widget> Application::locateWidgetInWindow(const std::shared_ptr<Window>& window, const math::float2& screenSpacePosition)
    {
		UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
		//find window widget
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

//        if(windowWidget == nullptr)
//        {
//            //android情况，由于进入前后台窗口重新创建了
//            windowWidgets[0]->assocateWithNativeWindow(window);
//            windowWidget = windowWidgets[0];
//        }
        std::vector<std::shared_ptr<Widget>> allWidgets = uiRenderPass->getAllWidgets(windowWidget);
        //assert(windowWidget->getNativeWindow() == window);//todo:fix this
        //if(windowWidget->getNativeWindow() != window)
        //{
        //    windowWidget->assocateWithNativeWindow(window);//todo:这里需要修复，不应该在这里设置native window 的
        //}

        std::stable_sort(allWidgets.begin(), allWidgets.end(), [&](const std::shared_ptr<Widget>& lhs, const std::shared_ptr<Widget>& rhs)
            {
                return lhs->getLayer() < rhs->getLayer();
            });

        math::float2 cursorPosition = screenSpacePosition - window->getWindowScreenSpacePosition();
        std::shared_ptr<Widget> collisionWidget;
        for (int32_t i = allWidgets.size() - 1; i > 0; --i)
        {
			//is Hit Test visible 在原版里面是在添加进 hittest grid 的时候，进行过滤
			if (allWidgets[i]->getVisibility().isHitTestVisible())
			{
				WidgetGeometry widgetGeometry = allWidgets[i]->getWidgetGeometry();

				math::float2 localSize = widgetGeometry.getLocalSize();
				math::affine2 transform = widgetGeometry.getAccumulateLayoutTransform();

				math::float2 topLeft = transform.transformPoint(math::float2(0.0f, 0.0f));
				math::float2 topRight = transform.transformPoint(math::float2(localSize.x, 0.0f));
				math::float2 bottomLeft = transform.transformPoint(math::float2(0.0f, localSize.y));
				math::float2 bottomRight = transform.transformPoint(math::float2(localSize.x, localSize.y));

				math::float2 points[4] = {
					 topLeft,
					 topRight,
					 bottomRight,
					 bottomLeft
				};

				float a = math::cross(points[1] - points[0], cursorPosition - points[0]);
				float b = math::cross(points[2] - points[1], cursorPosition - points[1]);
				float c = math::cross(points[3] - points[2], cursorPosition - points[2]);
				float d = math::cross(points[0] - points[3], cursorPosition - points[3]);

				if ((a > 0 && b > 0 && c > 0 && d > 0) || (a < 0 && b < 0 && c < 0 && d < 0))
				{
					collisionWidget = allWidgets[i];
					break;
				}
			}           
        }

		//GuGu_LOGD("%s", collisionWidget->getSlotsNumber())
        return collisionWidget;
    }

	math::float2 Application::getCursorPos() const
	{
		return m_lastCursorPos;
	}

    math::float2 Application::translateCursorPos(math::float2 cursorPos, std::shared_ptr<WindowWidget> inWindowWidget)
    {
        DeviceManager* deviceManager = m_renderer->getDeviceManager();
        uint32_t width = inWindowWidget->getFixedSize().x;
        uint32_t height = inWindowWidget->getFixedSize().y;

        math::float4x4 viewport = math::viewportMatrix(width, height);
		math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
		math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
		math::float3 cameraUp = math::float3(0.0f, -1.0f, 0.0f);
		math::float3 cameraRight = normalize(cross(cameraDir, cameraUp));
		cameraUp = normalize(cross(cameraRight, cameraDir));
		math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
		math::affine3 preRotateWorldToView = worldToView * math::rotation(math::normalize(math::float3(0.0f, 0.0f, 1.0f)), math::radians(getGlobalPreRotate()));
		math::float4x4 projMatrix = math::orthoProjD3DStyle(0, width, 0, height, 0, 1);
		math::float4x4 vp = projMatrix * math::affineToHomogeneous(preRotateWorldToView);
        math::float4x4 vpInverse = math::inverse(vp);
        math::float4 originCursor = math::inverse(viewport) * math::float4(cursorPos.x, cursorPos.y, 0.0f, 1.0f) * vpInverse;

        //git rid of rotate
        math::affine3 negPreRotateWorldToView = worldToView * math::rotation(math::normalize(math::float3(0.0f, 0.0f, 1.0f)), math::radians(-getGlobalPreRotate()));
        vp = projMatrix * math::affineToHomogeneous(negPreRotateWorldToView);
		math::float4 tempPoint = originCursor * vp;
		tempPoint = math::float4(tempPoint.x / tempPoint.w, tempPoint.y / tempPoint.w, tempPoint.z / tempPoint.w, tempPoint.w / tempPoint.w);
        math::float4 newCursor = math::viewportMatrix(width, height) * tempPoint;
        return newCursor;
    }

	std::shared_ptr<IToolTip> Application::makeToolTip(const Attribute<GuGuUtf8Str>& toolTipText)
	{
		return WIDGET_NEW(ToolTip)
			.text(toolTipText);
	}
	std::shared_ptr<IToolTip> Application::makeToolTip(const GuGuUtf8Str& toolTipText)
	{
		return WIDGET_NEW(ToolTip)
			.text(toolTipText);
	}

	void Application::updateToolTip(const MenuStack& menuStack, bool bCanSpawnNewToolTip)
	{
		WidgetPath widgetsToQueryForToolTip;

		WidgetPath widgetsUnderCursor = locateWidgetUnderMouse(getCursorPos(), m_windowWidgets);
		if (widgetsUnderCursor.isValid())
		{
			widgetsToQueryForToolTip = widgetsUnderCursor;
		}

		std::shared_ptr<IToolTip> newToolTip;
		std::shared_ptr<Widget> widgetProvidingNewToolTip;
		int32_t widgetNumber = widgetsToQueryForToolTip.m_widgets.getArrangedWidgetsNumber() - 1;
		for (int32_t widgetIndex = widgetNumber; widgetIndex >= 0; --widgetIndex)
		{
			std::shared_ptr<ArrangedWidget> arrangedWidget = widgetsToQueryForToolTip.m_widgets.getArrangedWidget(widgetIndex);
			const std::shared_ptr<Widget>& curWidget = arrangedWidget->getWidget();

			if (!newToolTip)
			{
				std::shared_ptr<IToolTip> widgetToolTip = curWidget->getToolTip();
				if (widgetToolTip && !widgetToolTip->isEmpty())
				{
					widgetProvidingNewToolTip = curWidget;
					newToolTip = widgetToolTip;
				}
			}
		}

		std::shared_ptr<IToolTip> activeToolTip = m_activeTooltipInfo.m_toolTip.lock();
		const bool bTooltipChanged = newToolTip != activeToolTip;
		if (bTooltipChanged)
		{
			if (newToolTip)
			{
				newToolTip->onOpening();
			}
		}

		math::float2 desiredLocation = m_activeTooltipInfo.m_desiredLocation;
		if (newToolTip && newToolTip != activeToolTip)
		{
			desiredLocation = m_lastCursorPos + math::float2(12.0f, 8.0f);
		}

		if (m_toolTipWindowPtr.lock())
		{
			//计算合适的弹出位置
		}

		if (bTooltipChanged)
		{
			if (newToolTip || (activeToolTip && !activeToolTip->isInteractive()))
			{
				closeTooltip();

				if (bCanSpawnNewToolTip && newToolTip)
				{
					showTooltip(newToolTip, desiredLocation);
					m_activeTooltipInfo.m_sourceWidget = widgetProvidingNewToolTip;
				}
			}
		}
	}

	WidgetPath Application::locateWidgetUnderMouse(math::float2 screenSpaceMouseCoordinate, const std::vector<std::shared_ptr<WindowWidget>>& windows)
	{
		for (int32_t windowIndex = windows.size() - 1; windowIndex >= 0; --windowIndex)
		{
			const std::shared_ptr<WindowWidget>& window = windows[windowIndex];

			//todo:检查是否处于最小化，或者可见
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window->getNativeWindow(), screenSpaceMouseCoordinate);

			if (collisionWidget)
			{
				std::vector<std::shared_ptr<Widget>> widgets;
				std::shared_ptr<Widget> currentWidget = collisionWidget;
				while (currentWidget)
				{
					widgets.push_back(currentWidget);
					currentWidget = currentWidget->getParentWidget();
				}
				std::reverse(widgets.begin(), widgets.end()); //构造widget path
				WidgetGeometry windowOffsetGeometry;//窗口左上角坐标
				windowOffsetGeometry.mAbsolutePosition = window->getNativeWindow()->getWindowScreenSpacePosition();
				WidgetPath widgetPath(widgets, windowOffsetGeometry);
				return widgetPath;
			}		
		}
		return WidgetPath();
	}

	void Application::ActiveTooltipInfo::reset()
	{
		if (m_sourceWidget.lock())
		{
			//
		}

		if (m_toolTip.lock())
		{
			m_toolTip.lock()->onClosed();
		}

		m_toolTip.reset();
		m_sourceWidget.reset();
	}

	//void Application::resize(int32_t width, int32_t height) {
//
    //}
}