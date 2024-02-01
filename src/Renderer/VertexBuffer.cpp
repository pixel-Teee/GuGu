#include <pch.h>

#include <Renderer/DeviceManager.h>

#include <Renderer/matrix.h>
#include <Renderer/utils.h>

namespace GuGu{
    constexpr uint32_t c_NumViews = 4;

    class VertexBuffer : public IRenderPass{
    private:
        nvrhi::CommandListHandle m_CommandList;
        nvrhi::BufferHandle m_ConstantBuffer;
    public:
        using IRenderPass::IRenderPass;

        struct ConstantBufferEntry
        {
            dm::float4x4 viewProjMatrix;
            float padding[16*3];
        };

        bool Init()
        {
            m_ConstantBuffer = GetDevice()->createBuffer(nvrhi::utils::CreateStaticConstantBufferDesc(sizeof(ConstantBufferEntry) * c_NumViews, "ConstantBuffer")
                                                                 .setInitialState(nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(true));

            m_CommandList = GetDevice()->createCommandList();
        }

        void Animate()
        {

        }

        void BackBufferResizing() override
        {

        }

        void Render(nvrhi::IFramebuffer* framebuffer) override
        {

        }


    };

    void test()
    {
        GuGu::nvrhi::GraphicsAPI api = GuGu::nvrhi::GraphicsAPI::VULKAN;
        DeviceManager* deviceManager = GuGu::DeviceManager::Create(api); //todo:delete this device manager
        GuGu::DeviceCreationParameters deviceParams;
#ifdef _DEBUG
        deviceParams.enableDebugRuntime = true;
        deviceParams.enableNvrhiValidationLayer = true;
#endif
        GuGuUtf8Str windowTitle = "VulkanApp";
        if(!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, windowTitle))
        {
            GuGu_LOGE("Cannot initialize a graphics device with the requested parameters");
            return;
        }

        {
            VertexBuffer example(deviceManager);
            if(example.Init())
            {
                deviceManager->AddRenderPassToBack(&example);
                deviceManager->RunMessageLoop();
                deviceManager->RemoveRenderPass(&example);
            }
        }

        deviceManager->ShutDown();

        delete deviceManager;
    }
}