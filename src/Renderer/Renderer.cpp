#include <pch.h>

#include "Renderer.h"
#include "Application.h"
#include <Core/UI/WindowWidget.h>
#include <Renderer/DeviceManager.h>

namespace GuGu {
	Renderer::Renderer()
	{
	}
	Renderer::~Renderer()
	{
	}
	void Renderer::init(std::shared_ptr<WindowWidget> inWindowWidget)
	{
		//nothing
	}
	void Renderer::onRender()
	{
	}
	void Renderer::onDestroy()
	{
	}
    void Renderer::onResize(int32_t width, int32_t height)
	{
    }
	void Renderer::requestResize(std::shared_ptr<Window> inWindow)
	{
		std::shared_ptr<Application> application = Application::getApplication();
		std::vector<std::shared_ptr<WindowWidget>> windowWidgets = application->getWidowWidgets();
		//find window widget
		for (size_t i = 0; i < windowWidgets.size(); ++i)
		{
			if (windowWidgets[i]->getNativeWindow() == inWindow)
			{
				m_deviceManager->ResizeSwapChain(windowWidgets[i]);
				break;
			}
		}
	}
	UIRenderPass* Renderer::getUIRenderPass() const
	{
		return m_UIRenderPass;
	}
	Demo* Renderer::getDemoPass() const
	{
		return m_demo;
	}
	VertexBuffer* Renderer::getVertexBufferPass() const
	{
		return m_vertexBuffer;
	}
	DeviceManager* Renderer::getDeviceManager() const
	{
		return m_deviceManager;
	}
	void Renderer::createSurface(std::shared_ptr<WindowWidget> windowWidget)
	{
	}
	void Renderer::createSwapChain(std::shared_ptr<WindowWidget> windowWidget)
	{
	}
	void Renderer::onWindowDestroyed(const std::shared_ptr<WindowWidget>& inWindow)
	{
		m_deviceManager->onWindowDestroyed(inWindow);
	}
}