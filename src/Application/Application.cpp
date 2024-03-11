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

        PointerEvent mouseEvent(
            cursorPos,
            m_lastCursorPos
        );
        m_lastCursorPos = cursorPos;

        return processMouseButtonDownEvent(window, mouseEvent);
	}

    bool Application::onMouseUp(const std::shared_ptr<Window>& window, math::float2 cursorPos)
    {
		PointerEvent mouseEvent(
			cursorPos,
			m_lastCursorPos
		);
		m_lastCursorPos = cursorPos;

        return processMouseButtonUpEvent(window, mouseEvent);
    }

    bool Application::processMouseButtonDownEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
        //locate window under mouse
        UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
        std::shared_ptr<WindowWidget> windowWidget = uiRenderPass->getWindowWidget();
        std::vector<std::shared_ptr<Widget>> allWidgets = uiRenderPass->getAllWidgets();
        assert(windowWidget->getNativeWindow() == window);//todo:fix this

        std::stable_sort(allWidgets.begin(), allWidgets.end(), [&](const std::shared_ptr<Widget>& lhs, const std::shared_ptr<Widget>& rhs)
            {
                return lhs->getLayer() < rhs->getLayer();
            });

        math::float2 cursorPosition = mouseEvent.m_screenSpacePosition - window->getWindowScreenSpacePosition();
        std::shared_ptr<Widget> collisionWidget;
        for (uint32_t i = allWidgets.size() - 1; i > 0; --i)
        {
            WidgetGeometry widgetGeometry = allWidgets[i]->getWidgetGeometry();

            math::float2 points[4] = {
                math::float2(widgetGeometry.getAbsolutePosition().x, widgetGeometry.getAbsolutePosition().y),//top left
                math::float2(widgetGeometry.getAbsolutePosition().x + widgetGeometry.getLocalSize().x, widgetGeometry.getAbsolutePosition().y),//top right
                math::float2(widgetGeometry.getAbsolutePosition().x + widgetGeometry.getLocalSize().x, widgetGeometry.getAbsolutePosition().y + widgetGeometry.getLocalSize().y),//bottom right
                math::float2(widgetGeometry.getAbsolutePosition().x, widgetGeometry.getAbsolutePosition().y + widgetGeometry.getLocalSize().y)//bottom left
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

        if (collisionWidget)
        {
            collisionWidget->OnMouseButtonDown(collisionWidget->getWidgetGeometry(), mouseEvent);
			//std::shared_ptr<ImageWidget> imageWidget = std::dynamic_pointer_cast<ImageWidget>(collisionWidget);
			//if (imageWidget)
			//{
			//    GuGu_LOGD("%s", u8"image widget");
			//}
        }
        //GuGu_LOGD("(%f %f)", cursorPosition.x, cursorPosition.y);
        return true;
    }

    bool Application::processMouseButtonUpEvent(const std::shared_ptr<Window>& window, const PointerEvent& mouseEvent)
    {
		//locate window under mouse
		UIRenderPass* uiRenderPass = m_renderer->getUIRenderPass();
		std::shared_ptr<WindowWidget> windowWidget = uiRenderPass->getWindowWidget();
		std::vector<std::shared_ptr<Widget>> allWidgets = uiRenderPass->getAllWidgets();
		assert(windowWidget->getNativeWindow() == window);//todo:fix this

		std::stable_sort(allWidgets.begin(), allWidgets.end(), [&](const std::shared_ptr<Widget>& lhs, const std::shared_ptr<Widget>& rhs)
			{
				return lhs->getLayer() < rhs->getLayer();
			});

		math::float2 cursorPosition = mouseEvent.m_screenSpacePosition - window->getWindowScreenSpacePosition();
		std::shared_ptr<Widget> collisionWidget;
		for (uint32_t i = allWidgets.size() - 1; i > 0; --i)
		{
			WidgetGeometry widgetGeometry = allWidgets[i]->getWidgetGeometry();

			math::float2 points[4] = {
				math::float2(widgetGeometry.getAbsolutePosition().x, widgetGeometry.getAbsolutePosition().y),//top left
				math::float2(widgetGeometry.getAbsolutePosition().x + widgetGeometry.getLocalSize().x, widgetGeometry.getAbsolutePosition().y),//top right
				math::float2(widgetGeometry.getAbsolutePosition().x + widgetGeometry.getLocalSize().x, widgetGeometry.getAbsolutePosition().y + widgetGeometry.getLocalSize().y),//bottom right
				math::float2(widgetGeometry.getAbsolutePosition().x, widgetGeometry.getAbsolutePosition().y + widgetGeometry.getLocalSize().y)//bottom left
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

		if (collisionWidget)
		{
			collisionWidget->OnMouseButtonUp(collisionWidget->getWidgetGeometry(), mouseEvent);
			//std::shared_ptr<ImageWidget> imageWidget = std::dynamic_pointer_cast<ImageWidget>(collisionWidget);
			//if (imageWidget)
			//{
			//    GuGu_LOGD("%s", u8"image widget");
			//}
		}
		GuGu_LOGD("(%f %f)", cursorPosition.x, cursorPosition.y);
		return true;
    }

    //void Application::resize(int32_t width, int32_t height) {
//
    //}
}