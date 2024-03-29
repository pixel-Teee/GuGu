#include <pch.h>

#include "Application.h"

#include <Core/Timer.h>
#include <Core/UI/Events.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/WindowWidget.h>
#include <Core/UI/UIRenderPass.h>
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
		KeyEvent keyEvent(key, characterCode, keyCode);

		return processKeyDownEvent(keyEvent);
	}

    std::shared_ptr<Widget> Application::getCaptorWidget() const
    {
        if (!m_captorWidgetsPath.empty())
            return m_captorWidgetsPath.back().lock();
        return nullptr;
    }

	bool Application::hasAnyFocus(std::shared_ptr<const Widget> inWidget) const
	{
		if(!m_focusWidgetsPath.empty())
			return m_focusWidgetsPath.back().lock() == inWidget;
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

    bool Application::processMouseButtonDownEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		if (!m_captorWidgetsPath.empty())
		{
			std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
			for (int32_t i = captorWidgetsPath.size() - 1; i >= 0; --i)
			{
				std::shared_ptr<Widget> widget = captorWidgetsPath[i].lock();
				if (widget != nullptr)
				{
					Reply reply = widget->OnMouseButtonDown(widget->getWidgetGeometry(), mouseEvent);
					std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					if (mouseCaptor != nullptr)
					{
						//m_captorWidget = mouseCaptor;
						m_captorWidgetsPath.clear();
						for (size_t j = 0; j <= i; ++j)
							m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					}
					if (reply.shouldReleaseMouse())
					{
						m_captorWidgetsPath.clear();
					}
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

			for (int32_t i = widgets.size() - 1; i >= 0; --i)
			{
				Reply reply = widgets[i]->OnMouseButtonDown(widgets[i]->getWidgetGeometry(), mouseEvent);

				std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				if (mouseCaptor != nullptr)
				{
					//m_captorWidget = mouseCaptor;
					m_captorWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_captorWidgetsPath.push_back(widgets[i]);
				}
				if (reply.shouldReleaseMouse())
				{
					m_captorWidgetsPath.clear();
				}
				std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				if (requestedFocusRecepient)
				{
					m_focusWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_focusWidgetsPath.push_back(widgets[i]);
				}
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

    bool Application::processMouseButtonUpEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		if (!m_captorWidgetsPath.empty())
		{
			std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
			for (int32_t i = captorWidgetsPath.size() - 1; i >= 0; --i)
			{
				std::shared_ptr<Widget> widget = captorWidgetsPath[i].lock();
				if (widget != nullptr)
				{
					Reply reply = widget->OnMouseButtonUp(widget->getWidgetGeometry(), mouseEvent);
					std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					if (mouseCaptor != nullptr)
					{
						//m_captorWidget = mouseCaptor;
						m_captorWidgetsPath.clear();
						for (size_t j = 0; j <= i; ++j)
							m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					}
					if (reply.shouldReleaseMouse())
					{
						m_captorWidgetsPath.clear();
					}
				}
			}
		}
        else
        {
			//locate window under mouse
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent);

			std::vector<std::shared_ptr<Widget>> widgets;
			std::shared_ptr<Widget> currentWidget = collisionWidget;
			while (currentWidget)
			{
				widgets.push_back(currentWidget);
				currentWidget = currentWidget->getParentWidget();
			}

			for (int32_t i = widgets.size() - 1; i >= 0; --i)
			{
				Reply reply = widgets[i]->OnMouseButtonUp(widgets[i]->getWidgetGeometry(), mouseEvent);

				std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				if (mouseCaptor != nullptr)
				{
					//m_captorWidget = mouseCaptor;
					m_captorWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_captorWidgetsPath.push_back(widgets[i]);
				}
				if (reply.shouldReleaseMouse())
				{
					m_captorWidgetsPath.clear();
				}
				std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				if (requestedFocusRecepient)
				{
					m_focusWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_focusWidgetsPath.push_back(widgets[i]);
				}
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

        if (!m_captorWidgetsPath.empty())
        {
            std::vector<std::weak_ptr<Widget>> captorWidgetsPath = m_captorWidgetsPath;
            for (int32_t i = captorWidgetsPath.size() - 1; i >= 0; --i)
            {
                std::shared_ptr<Widget> widget = captorWidgetsPath[i].lock();
                if (widget != nullptr)
                {
                    Reply reply = widget->OnMouseMove(widget->getWidgetGeometry(), mouseEvent);
					std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
					if (mouseCaptor != nullptr)
					{
						//m_captorWidget = mouseCaptor;
						m_captorWidgetsPath.clear();
						for (size_t j = 0; j <= i; ++j)
							m_captorWidgetsPath.push_back(captorWidgetsPath[j]);
					}
					if (reply.shouldReleaseMouse())
					{
						m_captorWidgetsPath.clear();
					}
                }
            }
        }
        else
        {
			//locate window under mouse
			std::shared_ptr<Widget> collisionWidget = locateWidgetInWindow(window, mouseEvent);

			std::vector<std::shared_ptr<Widget>> widgets;
			std::shared_ptr<Widget> currentWidget = collisionWidget;
			while (currentWidget)
			{
				widgets.push_back(currentWidget);
				currentWidget = currentWidget->getParentWidget();
			}

			for (int32_t i = widgets.size() - 1; i >= 0; --i)
			{
				Reply reply = widgets[i]->OnMouseMove(widgets[i]->getWidgetGeometry(), mouseEvent);

				std::shared_ptr<Widget> mouseCaptor = reply.getMouseCaptor();
				if (mouseCaptor != nullptr)
				{
					//m_captorWidget = mouseCaptor;
					m_captorWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_captorWidgetsPath.push_back(widgets[i]);
				}
				if (reply.shouldReleaseMouse())
				{
					m_captorWidgetsPath.clear();
				}
				std::shared_ptr<Widget> requestedFocusRecepient = reply.getFocusRecepient();
				if (requestedFocusRecepient)
				{
					m_focusWidgetsPath.clear();
					for (size_t j = 0; j <= i; ++j)
						m_focusWidgetsPath.push_back(widgets[i]);
				}
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
		std::vector<std::shared_ptr<Widget>> focusPath;
		for (size_t i = 0; i < m_focusWidgetsPath.size(); ++i)
			focusPath.push_back(m_focusWidgetsPath[i].lock());

		for (size_t i = 0; i < focusPath.size(); ++i)
			focusPath[i]->OnKeyChar(focusPath[i]->getWidgetGeometry(), inCharacterEvent);

		return true;
	}

	bool Application::processKeyDownEvent(const KeyEvent& inKeyEvent)
	{
		Reply reply = Reply::Unhandled();

		//focus path
		std::vector<std::shared_ptr<Widget>> focusPath;
		for (size_t i = 0; i < m_focusWidgetsPath.size(); ++i)
			focusPath.push_back(m_focusWidgetsPath[i].lock());

		for (size_t i = 0; i < focusPath.size(); ++i)
			focusPath[i]->OnKeyDown(focusPath[i]->getWidgetGeometry(), inKeyEvent);

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
            windowWidget->assocateWithNativeWindow(window);//todo:这里需要修复，不应该在这里设置native window的
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
            math::affine2 transform = widgetGeometry.getAccumulateTransform();

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