#include <pch.h>

#include <Renderer/DeviceManager.h>

#include <Renderer/utils.h>
#include <Renderer/MyMath.h>

namespace GuGu{
    constexpr uint32_t c_NumViews = 4;

    struct Vertex
    {
        math::float3 position;
        math::float2 uv;
    };

    static const Vertex g_Vertices[] = {
            { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} }, // front face
            { { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
            { {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
            { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

            { { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} }, // right side face
            { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
            { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
            { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} },

            { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // left side face
            { {-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
            { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
            { {-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

            { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // back face
            { {-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
            { { 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
            { {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },

            { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f} }, // top face
            { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
            { { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f} },
            { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} },

            { { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} }, // bottom face
            { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f} },
            { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f} },
            { {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
    };

    static const uint32_t g_Indices[] = {
            0,  1,  2,   0,  3,  1, // front face
            4,  5,  6,   4,  7,  5, // left face
            8,  9, 10,   8, 11,  9, // right face
            12, 13, 14,  12, 15, 13, // back face
            16, 17, 18,  16, 19, 17, // top face
            20, 21, 22,  20, 23, 21, // bottom face
    };

    class VertexBuffer : public IRenderPass{
    private:
        nvrhi::CommandListHandle m_CommandList;
        nvrhi::BufferHandle m_ConstantBuffer;
        nvrhi::InputLayoutHandle m_InputLayout;
        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSets[c_NumViews];
        //nvrhi::GraphicsPipelineHandle m_Pipeline;
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

            nvrhi::VertexAttributeDesc attributes[] = {
                    nvrhi::VertexAttributeDesc()
                            .setName("POSITION")
                            .setFormat(nvrhi::Format::RGB32_FLOAT)
                            .setOffset(0)
                            .setBufferIndex(0)
                            .setElementStride(sizeof(Vertex)),
                    nvrhi::VertexAttributeDesc()
                            .setName("UV")
                            .setFormat(nvrhi::Format::RG32_FLOAT)
                            .setOffset(0)
                            .setBufferIndex(1)
                            .setElementStride(sizeof(Vertex)),
            };

            //todo:add create input layout

            m_CommandList = GetDevice()->createCommandList();
            m_CommandList->open();

            nvrhi::BufferDesc vertexBufferDesc;
            vertexBufferDesc.byteSize = sizeof(g_Vertices);
            vertexBufferDesc.isVertexBuffer = true;
            vertexBufferDesc.debugName = "VertexBuffer";
            vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
            m_VertexBuffer = GetDevice()->createBuffer(vertexBufferDesc);

            //m_CommandList->beginTrackingBufferState(m_VertexBuffer, nvrhi::ResourceStates::CopyDest);
            //m_CommandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));
            //m_CommandList->setPermanentBufferState(m_VertexBuffer, nvrhi::ResourceStates::VertexBuffer);

            nvrhi::BufferDesc indexBufferDesc;
            indexBufferDesc.byteSize = sizeof(g_Indices);
            indexBufferDesc.isIndexBuffer = true;
            indexBufferDesc.debugName = "IndexBuffer";
            indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
            m_IndexBuffer = GetDevice()->createBuffer(indexBufferDesc);

            //m_CommandList->beginTrackingBufferState(m_IndexBuffer, nvrhi::ResourceStates::CopyDest);
            //m_CommandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));
            //m_CommandList->setPermanentBufferState(m_IndexBuffer, nvrhi::ResourceStates::IndexBuffer);

            //todo:load texture

            m_CommandList->close();
            //todo:add execute command list

            // Create a single binding layout and multiple binding sets, one set per view.
            // The different binding sets use different slices of the same constant buffer.
            for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex)
            {
                nvrhi::BindingSetDesc bindingSetDesc;
                bindingSetDesc.bindings = {
                        // Note: using viewIndex to construct a buffer range.
                        nvrhi::BindingSetItem::ConstantBuffer(0, m_ConstantBuffer, nvrhi::BufferRange(sizeof(ConstantBufferEntry) * viewIndex, sizeof(ConstantBufferEntry))),
                        // Texutre and sampler are the same for all model views.
                        //nvrhi::BindingSetItem::Texture_SRV(0, m_Texture),
                        //nvrhi::BindingSetItem::Sampler(0, commonPasses.m_AnisotropicWrapSampler)
                };

                // Create the binding layout (if it's empty -- so, on the first iteration) and the binding set.
                if (!nvrhi::utils::CreateBindingSetAndLayout(GetDevice(), nvrhi::ShaderType::All, 0, bindingSetDesc, m_BindingLayout, m_BindingSets[viewIndex]))
                {
                    GuGu_LOGD("Couldn't create the binding set or layout");
                    return false;
                }
            }

            return true;
        }

        void Animate()
        {

        }

        void BackBufferResizing() override
        {

        }

        void Render(nvrhi::IFramebuffer* framebuffer) override
        {
            const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();
            //if (!m_Pipeline)
            {
                //nvrhi::GraphicsPipelineDesc psoDesc;
                //psoDesc.VS = m_VertexShader;
                //psoDesc.PS = m_PixelShader;
                //psoDesc.inputLayout = m_InputLayout;
                //psoDesc.bindingLayouts = { m_BindingLayout };
                //psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
                //psoDesc.renderState.depthStencilState.depthTestEnable = false;

                //m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
            }

            m_CommandList->open();

            //nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(0.f));

            // Fill out the constant buffer slices for multiple views of the model.
            ConstantBufferEntry modelConstants[c_NumViews];
            for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex)
            {
                //math::affine3 viewMatrix = math::rotation(normalize(g_RotationAxes[viewIndex]), m_Rotation)
                //                           * math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
                //                           * math::translation(math::float3(0, 0, 2));
                //math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(60.f), float(fbinfo.width) / float(fbinfo.height), 0.1f, 10.f);
                //math::float4x4 viewProjMatrix = math::affineToHomogeneous(viewMatrix) * projMatrix;
                //modelConstants[viewIndex].viewProjMatrix = viewProjMatrix;
            }

            // Upload all constant buffer slices at once.
            //m_CommandList->writeBuffer(m_ConstantBuffer, modelConstants, sizeof(modelConstants));

            for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex)
            {
                //nvrhi::GraphicsState state;
                // Pick the right binding set for this view.
                //state.bindings = { m_BindingSets[viewIndex] };
                //state.indexBuffer = { m_IndexBuffer, nvrhi::Format::R32_UINT, 0 };
                // Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
                //state.vertexBuffers = {
                //        { m_VertexBuffer, 1, offsetof(Vertex, uv) },
                //        { m_VertexBuffer, 0, offsetof(Vertex, position) }
                //};
                //state.pipeline = m_Pipeline;
                //state.framebuffer = framebuffer;

                // Construct the viewport so that all viewports form a grid.
                const float width = float(fbinfo.width) * 0.5f;
                const float height = float(fbinfo.height) * 0.5f;
                const float left = width * float(viewIndex % 2);
                const float top = height * float(viewIndex / 2);

                //const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top, top + height, 0.f, 1.f);
                //state.viewport.addViewportAndScissorRect(viewport);

                // Update the pipeline, bindings, and other state.
                //m_CommandList->setGraphicsState(state);

                // Draw the model.
                //nvrhi::DrawArguments args;
                //args.vertexCount = dim(g_Indices);
                //m_CommandList->drawIndexed(args);
            }

            m_CommandList->close();
            //GetDevice()->executeCommandList(m_CommandList);
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