#pragma once

#include <Core/Math/MyMath.h>
#include <Renderer/DeviceManager.h>//IRenderPass

namespace GuGu {
    constexpr uint32_t
    c_NumViews = 4;

    struct Vertex {
        math::float3 position;
        math::float2 uv;
    };

    static const Vertex g_Vertices[] = {
            {{-0.5f, 0.5f,  -0.5f}, {0.0f, 0.0f}}, // front face
            {{0.5f,  -0.5f, -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 0.0f}},

            {{0.5f,  -0.5f, -0.5f}, {0.0f, 1.0f}}, // right side face
            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.5f},  {1.0f, 1.0f}},
            {{0.5f,  0.5f,  -0.5f}, {0.0f, 0.0f}},

            {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f}}, // left side face
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f, 0.5f},  {0.0f, 1.0f}},
            {{-0.5f, 0.5f,  -0.5f}, {1.0f, 0.0f}},

            {{0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}}, // back face
            {{-0.5f, -0.5f, 0.5f},  {1.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.5f},  {0.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.5f},  {1.0f, 0.0f}},

            {{-0.5f, 0.5f,  -0.5f}, {0.0f, 1.0f}}, // top face
            {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
            {{0.5f,  0.5f,  -0.5f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.5f},  {0.0f, 0.0f}},

            {{0.5f,  -0.5f, 0.5f},  {1.0f, 1.0f}}, // bottom face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
            {{0.5f,  -0.5f, -0.5f}, {1.0f, 0.0f}},
            {{-0.5f, -0.5f, 0.5f},  {0.0f, 1.0f}},
    };

    static const uint32_t g_Indices[] = {
            0, 1, 2, 0, 3, 1, // front face
            4, 5, 6, 4, 7, 5, // left face
            8, 9, 10, 8, 11, 9, // right face
            12, 13, 14, 12, 15, 13, // back face
            16, 17, 18, 16, 19, 17, // top face
            20, 21, 22, 20, 23, 21, // bottom face
    };

    static const math::float3 g_RotationAxes[c_NumViews] = {
            math::float3(1.f, 0.f, 0.f),
            math::float3(0.f, 1.f, 0.f),
            math::float3(0.f, 0.f, 1.f),
            math::float3(1.f, 1.f, 1.f),
    };

    class RootFileSystem;
    class VertexBuffer : public IRenderPass {
    private:
        nvrhi::ShaderHandle m_VertexShader;
        nvrhi::ShaderHandle m_PixelShader;
        nvrhi::CommandListHandle m_CommandList;
        nvrhi::BufferHandle m_ConstantBuffer;
        nvrhi::InputLayoutHandle m_InputLayout;
        nvrhi::BufferHandle m_VertexBuffer;
        nvrhi::BufferHandle m_IndexBuffer;
        nvrhi::BindingLayoutHandle m_BindingLayout;
        nvrhi::BindingSetHandle m_BindingSets[c_NumViews];
        nvrhi::GraphicsPipelineHandle m_Pipeline;
        nvrhi::TextureHandle m_Texture;
        float m_Rotation = 0.f;
        std::shared_ptr<RootFileSystem> m_rootFileSystem;
    public:
        using IRenderPass::IRenderPass;

        struct ConstantBufferEntry {
            dm::float4x4 viewProjMatrix;
            float padding[16 * 3];
        };

        bool Init();

        void Update(float fElapsedTimeSeconds) override;

        void BackBufferResizing() override;

        void Render() override;

        nvrhi::TextureHandle getRenderTarget();

        math::uint2 m_renderTargetSize;
		nvrhi::TextureHandle m_renderTarget;
		nvrhi::TextureHandle m_depthTarget;
		nvrhi::FramebufferHandle m_frameBuffer;

		void initRenderTargetAndDepthTarget();
    };
}