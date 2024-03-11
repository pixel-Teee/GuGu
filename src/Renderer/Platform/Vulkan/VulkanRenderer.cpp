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

namespace GuGu{
    VulkanRenderer::VulkanRenderer() {

    }
    VulkanRenderer::~VulkanRenderer() {
        m_deviceManager->RemoveRenderPass(m_demo);
		//m_deviceManager->RemoveRenderPass(m_vertexBuffer);
		m_deviceManager->RemoveRenderPass(m_UIRenderPass);
        delete m_vertexBuffer;
        delete m_UIRenderPass;
        delete m_demo;
		m_deviceManager->ShutDown();
		delete m_deviceManager;
    }
    void VulkanRenderer::init() {
        GuGu::nvrhi::GraphicsAPI api = GuGu::nvrhi::GraphicsAPI::VULKAN;
        m_deviceManager = GuGu::DeviceManager::Create(api);
        GuGu::DeviceCreationParameters deviceParams;
#ifdef ANDROID
        deviceParams.vsyncEnabled = true;
#endif
#ifdef _DEBUG
        deviceParams.enableDebugRuntime = true;
        deviceParams.enableNvrhiValidationLayer = true;
#endif
        GuGuUtf8Str windowTitle = "VulkanApp";
        if(!m_deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, windowTitle))
        {
            GuGu_LOGE("cannot initialize a graphics device with the requested parameters");
            return;
        }
        m_vertexBuffer =  new VertexBuffer(m_deviceManager);
        m_UIRenderPass = new UIRenderPass(m_deviceManager);
        m_demo = new Demo(m_deviceManager);
		if (m_demo->Init())
		{
			m_deviceManager->AddRenderPassToBack(m_demo);
		}

		//if(m_vertexBuffer->Init())
		//{
		//    m_deviceManager->AddRenderPassToBack(m_vertexBuffer);
		//}
        if (m_UIRenderPass->Init())
        {
            m_deviceManager->AddRenderPassToBack(m_UIRenderPass);
        }
    }
    void VulkanRenderer::onRender() {
        m_deviceManager->RunMessageLoop();
    }
    void VulkanRenderer::onDestroy() {
        //static object need to destroy on this place
        StyleSet::getStyle()->clear();
        FontCache::getFontCache()->clear();
    }
}