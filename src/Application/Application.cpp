#include <pch.h>

#include "Application.h"

#include <Core/Timer.h>
#include <Core/UI/Events.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/WindowWidget.h>
#include <Core/UI/UIRenderPass.h>
#include <Core/UI/WidgetPath.h>
#include <Core/UI/ArrangedWidget.h>
#include <Core/UI/ArrangedWidgetArray.h>
#include <Window/Window.h>
#include <Renderer/Demo.h>
#include <Renderer/Renderer.h>
#include <Renderer/VertexBuffer.h>

namespace GuGu{
    Application::Application()
    {
        m_alreadyExit = false;
        m_focused = true;
        m_timer = CreateTimerFactory();
        m_lastCursorPos = math::float2(0.0f, 0.0f);
    }
	void Application::init()
	{
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
                UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
                Demo* demoPass = m_renderer->getDemoPass();
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

    float Application::getFps() const
    {
        return fps;
    }

    float Application::getmFps() const
    {
        return mfps;
    }

	bool Application::onMouseDown(const std::shared_ptr<Window>& window, math::float2 cursorPos)
	{
		//process mouse button down

        //transfer cursorPos to local space
        math::float2 translatedCursorPos = translateCursorPos(cursorPos);


        PointerEvent mouseEvent(
            translatedCursorPos,
            m_lastCursorPos
        );
        m_lastCursorPos = translatedCursorPos;

        return processMouseButtonDownEvent(window, mouseEvent);
	}

    bool Application::onMouseUp(const std::shared_ptr<Window>& window, math::float2 cursorPos)
    {
        math::float2 translatedCursorPos = translateCursorPos(cursorPos);

		PointerEvent mouseEvent(
            translatedCursorPos,
			m_lastCursorPos
		);
		m_lastCursorPos = translatedCursorPos;

        return processMouseButtonUpEvent(window, mouseEvent);
    }

    bool Application::onMouseMove(const std::shared_ptr<Window>& window, math::float2 cursorPos)
    {
		math::float2 translatedCursorPos = translateCursorPos(cursorPos);

		PointerEvent mouseEvent(
			translatedCursorPos,
			m_lastCursorPos
		);
		m_lastCursorPos = translatedCursorPos;

		return processMouseMoveEvent(window, mouseEvent);
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

    std::shared_ptr<Widget> Application::getCaptorWidget() const
    {
        if (!m_captorWidgetsPath.isEmpty())
            return m_captorWidgetsPath.getLastWidget().lock();
        return nullptr;
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

	void Application::processReply(const Reply& reply, const WidgetPath& widgetPath)
	{
		if (reply.shouldReleaseMouse())
		{
			m_captorWidgetsPath.clear();
		}
		std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
		if (mouseCaptor != nullptr)
		{
			//m_captorWidget = mouseCaptor;
			//m_captorWidgetsPath.clear();
			//for (int32_t j = i; j < widgets.size(); ++j)
			//	m_captorWidgetsPath.push_back(widgets[i]);
			m_captorWidgetsPath = widgetPath.pathDownTo(mouseCaptor);
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
		//记录旧的焦点路径
		WidgetPath oldFocusWidgetsPath;
		m_focusWidgetsPath.toWidgetPath(oldFocusWidgetsPath);

		std::shared_ptr<Widget> oldFocus;
		if (oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() != 0)
		{
			oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
		}

		//set user focus(这里是设置用户焦点的代码逻辑)
		//设置新的焦点控件
		size_t widgetNumber = widgetPath.m_widgets.getArrangedWidgetsNumber();
		for (int32_t i = widgetNumber - 1; i >= 0; --i)
		{
			std::shared_ptr<Widget> widget = widgetPath.m_widgets[i]->getWidget();
			if (widget->supportsKeyboardFocus())
			{
				WidgetPath focusedWidgetPath;
				focusedWidgetPath = widgetPath.pathDownTo(widget);

				if (widget == oldFocus) //focus 没变化，不调用onFocusLost和onFocusReceived等函数
				{
					return;
				}

				int32_t focusedWidgetNumber = focusedWidgetPath.m_widgets.getArrangedWidgetsNumber();
				for (int32_t j = focusedWidgetNumber - 1; j >= 0; --j)
				{
					std::shared_ptr<Widget> widget = focusedWidgetPath.m_widgets[j]->getWidget();
					m_focusWidgetsPath.clear();
					m_focusWidgetsPath = focusedWidgetPath.pathDownTo(widget);
					break;
				}

				if (oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() != 0)
				{
					//std::shared_ptr<Widget> oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
					oldFocus->OnFocusLost();
				}

				if (focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() != 0)
				{
					std::shared_ptr<Widget> newFocusWidget = focusedWidgetPath.m_widgets[focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
					newFocusWidget->OnFocusReceived(newFocusWidget->getWidgetGeometry());
				}

				//std::shared_ptr<Widget> oldFocus = oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
				//
				//oldFocusWidgetsPath.m_widgets[oldFocusWidgetsPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget()->OnFocusLost();
				//
				//std::shared_ptr<Widget> newFocusWidget = focusedWidgetPath.m_widgets[focusedWidgetPath.m_widgets.getArrangedWidgetsNumber() - 1]->getWidget();
				//newFocusWidget->OnFocusReceived(newFocusWidget->getWidgetGeometry());

				break;
			}
		}
	}

	std::shared_ptr<Widget> Application::getKeyboardFocusedWidget() const
	{
		if (m_focusWidgetsPath.isEmpty())
			return nullptr;
		return m_focusWidgetsPath.getLastWidget().lock();
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
				}
			}
		}
		else
        {
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent);

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
				}
			}
		}
		else
		{
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent);

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
				}
			}
		}
		else
		{
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent);

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

    std::shared_ptr<Widget> Application::locateWidgetInWindow(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
        UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
        std::shared_ptr<WindowWidget> windowWidget = uiRenderPass->getWindowWidget();
        std::vector<std::shared_ptr<Widget>> allWidgets = uiRenderPass->getAllWidgets();
        //assert(windowWidget->getNativeWindow() == window);//todo:fix this
        if(windowWidget->getNativeWindow() != window)
        {
            windowWidget->assocateWithNativeWindow(window);//todo:这里需要修复，不应该在这里设置native window 的
        }

        std::stable_sort(allWidgets.begin(), allWidgets.end(), [&](const std::shared_ptr<Widget>& lhs, const std::shared_ptr<Widget>& rhs)
            {
                return lhs->getLayer() < rhs->getLayer();
            });

        math::float2 cursorPosition = mouseEvent.m_screenSpacePosition - window->getWindowScreenSpacePosition();
        std::shared_ptr<Widget> collisionWidget;
        for (int32_t i = allWidgets.size() - 1; i > 0; --i)
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

		//GuGu_LOGD("%s", collisionWidget->getSlotsNumber())
        return collisionWidget;
    }

    math::float2 Application::translateCursorPos(math::float2 cursorPos)
    {
        DeviceManager* deviceManager = m_renderer->getDeviceManager();
        uint32_t width = deviceManager->getDeviceCreationParameters().backBufferWidth;
        uint32_t height = deviceManager->getDeviceCreationParameters().backBufferHeight;

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

    //void Application::resize(int32_t width, int32_t height) {
//
    //}
}