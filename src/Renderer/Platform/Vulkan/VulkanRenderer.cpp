#include <pch.h>

#include "VulkanRenderer.h"

#include <Core/GuGuUtf8Str.h>

#ifdef ANDROID
#include <Window/Platform/Android/AndroidWindow.h>
#include <Application/Platform/Android/AndroidApplication.h>
#else
    #ifdef WIN32
        #include <Window/Platform/Windows/WindowsWindow.h>
        #include <Application/Platform/Windows/WindowsApplication.h>        
    #endif
#endif

#include <Core/UI/StyleSet.h>
#include <Core/UI/FontCache.h>
#include <Core/UI/UIRenderPass.h>
#include <Renderer/DeviceManager.h>
#include <Renderer/VertexBuffer.h>
#include <Renderer/Demo.h>

#include <Core/UI/UIData.h>//todo:未来移除这个

namespace GuGu{

    VulkanRenderer::VulkanRenderer() {

    }
    VulkanRenderer::~VulkanRenderer() {
        m_deviceManager->RemoveRenderPass(m_demo);
		m_deviceManager->RemoveRenderPass(m_vertexBuffer);
		//m_deviceManager->RemoveRenderPass(m_UIRenderPass);
        delete m_vertexBuffer;
        delete m_UIRenderPass;
        delete m_demo;
		m_deviceManager->ShutDown();
		delete m_deviceManager;
    }
    void VulkanRenderer::init(std::shared_ptr<WindowWidget> inWindowWidget) {
		std::shared_ptr<UIData> uiData = std::make_shared<UIData>();

        //CreateWindowDeviceAndSwapChain 创建窗口依赖 uirenderpass 先创建
        GuGu::nvrhi::GraphicsAPI api = GuGu::nvrhi::GraphicsAPI::VULKAN;
        m_deviceManager = GuGu::DeviceManager::Create(api);
        GuGu::DeviceCreationParameters deviceParams;
#ifdef ANDROID
        deviceParams.vsyncEnabled = true;
#endif
#ifdef _DEBUG
        deviceParams.enableDebugRuntime = true;
        deviceParams.enableNvrhiValidationLayer = false;
#endif
#ifdef ANDROID //android无法开启debug runtime
        deviceParams.enableDebugRuntime = false;
        deviceParams.enableNvrhiValidationLayer = false;
#endif
        GuGuUtf8Str windowTitle = "VulkanApp";
        if(!m_deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, windowTitle, inWindowWidget))
        {
            GuGu_LOGE("cannot initialize a graphics device with the requested parameters");
            return;
        }

		m_vertexBuffer = new VertexBuffer(m_deviceManager);
		m_UIRenderPass = new UIRenderPass(m_deviceManager);
		m_vertexBuffer = new VertexBuffer(m_deviceManager);
		m_demo = new Demo(m_deviceManager);

        //m_UIRenderPass->addWindowWidget(m_deviceManager->getWindowWidget());//main window widget

		if (m_demo->Init(uiData))
		{
			m_deviceManager->AddRenderPassToBack(m_demo);
		}

		if (m_vertexBuffer->Init())
		{
			m_deviceManager->AddRenderPassToBack(m_vertexBuffer);
		}
		if (m_UIRenderPass->Init(uiData))
		{
			//m_deviceManager->AddRenderPassToBack(m_UIRenderPass);
		}
    }
    void VulkanRenderer::onRender() {
        m_deviceManager->RunMessageLoop();
    }
    void VulkanRenderer::onDestroy() {
        //static object need to destroy on this place
        CoreStyle::getStyleSet()->clear();
        FontCache::getFontCache()->clear();
    }
    void VulkanRenderer::createSurface(std::shared_ptr<WindowWidget> windowWidget)
    {
        m_deviceManager->createWindowSurface(windowWidget);
    }
    void VulkanRenderer::createSwapChain(std::shared_ptr<WindowWidget> windowWidget)
    {
        //'m_deviceManager->CreateSwapChain();
        m_deviceManager->CreateSwapChain(windowWidget);
    }
    void VulkanRenderer::destoryOldSwapChain(std::shared_ptr<WindowWidget> windowWidget)
    {
        m_deviceManager->destroyOldSwapChain(windowWidget);
    }
    void VulkanRenderer::resizeSwapChain(std::shared_ptr<WindowWidget> windowWidget)
    {
        m_deviceManager->ResizeSwapChain(windowWidget);
    }
}