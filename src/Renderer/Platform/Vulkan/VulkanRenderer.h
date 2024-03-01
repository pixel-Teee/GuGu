#pragma once

#include "Renderer.h"

namespace GuGu{
    class DeviceManager;
    class VertexBuffer;
    class UIRenderPass;
    class Demo;
    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer();

        virtual ~VulkanRenderer();

        virtual void init() override;

        virtual void onRender() override;

        virtual void onDestroy() override;
    private:
        DeviceManager* m_deviceManager;//todo:remove this

		VertexBuffer* m_vertexBuffer;
                    
		UIRenderPass* m_UIRenderPass;

        Demo* m_demo;
    };
}