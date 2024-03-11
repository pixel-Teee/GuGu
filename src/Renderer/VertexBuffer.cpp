#include <pch.h>

#include "VertexBuffer.h"

#include <Renderer/utils.h>
#include <Renderer/CommonRenderPasses.h>
#include <Renderer/ShaderFactory.h>
#include <Renderer/TextureCache.h>

#include <Core/FileSystem/FileSystem.h>
#include <Application/Application.h>

namespace GuGu {
	bool VertexBuffer::Init()
	{
		GuGuUtf8Str assetPath = Application::GetDirectoryWithExecutable();
		//std::shared_ptr<NativeFileSystem> archiveNativeFileSystem = std::make_shared<NativeFileSystem>(assetPath);
		//std::shared_ptr<NativeFileSystem> archiveNativeFileSystem2 = std::make_shared<NativeFileSystem>(assetPath);
		//std::shared_ptr<RootFileSystem> rootFileSystem = std::make_shared<RootFileSystem>();
		//rootFileSystem->mount("/asset", archiveNativeFileSystem);
		//std::shared_ptr<RootFileSystem> rootFileSystem2 = std::make_shared<RootFileSystem>();
		//rootFileSystem2->mount("/asset", archiveNativeFileSystem2);
		//std::vector<GuGuUtf8Str> filePaths = {
		//	u8"biscuit.jpg",
		//	u8"CheckerBoard.png",
		//	u8"fun.jpg",
		//	u8"nvidia-logo.png",
		//	u8"shaders_main_vs.bin",
		//	u8"shaders_main_ps.bin",
		//	u8"STKAITI.TTF",
		//	u8"UIShader_main_vs.bin",
		//	u8"UIShader_main_ps.bin"
		//};
		//GuGuUtf8Str mountPoint = "/asset";
		//CreateArchiveFiles(rootFileSystem, rootFileSystem2, filePaths, mountPoint + u8"/archiver.bin", mountPoint);
#if 1
		std::shared_ptr<NativeFileSystem> nativeFileSystem = std::make_shared<NativeFileSystem>(assetPath);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("asset", nativeFileSystem);
#else
        std::shared_ptr<ArchiverFileSystem> archiverFileSystem;
        if(assetPath == "")
            archiverFileSystem = std::make_shared<ArchiverFileSystem>("archiver.bin");//this is root file
        else
            archiverFileSystem = std::make_shared<ArchiverFileSystem>(assetPath + "/archiver.bin");
        m_rootFileSystem = std::make_shared<RootFileSystem>();
        m_rootFileSystem->mount("asset", archiverFileSystem);
#endif	

		std::shared_ptr <ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>(
			GetDevice(), m_rootFileSystem);

		m_VertexShader = shaderFactory->CreateShader("asset/shaders.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_PixelShader = shaderFactory->CreateShader("asset/shaders.hlsl", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);

		if (!m_VertexShader || !m_PixelShader) {
			return false;
		}

		m_ConstantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(ConstantBufferEntry) * c_NumViews, "ConstantBuffer")
			.setInitialState(
				nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(
					true));

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
		m_InputLayout = GetDevice()->createInputLayout(attributes,
			uint32_t(std::size(attributes)),
			m_VertexShader);

		CommonRenderPasses commonPasses(GetDevice(), shaderFactory);
		TextureCache textureCache(GetDevice(), m_rootFileSystem);

		m_CommandList = GetDevice()->createCommandList();
		m_CommandList->open();

		nvrhi::BufferDesc vertexBufferDesc;
		vertexBufferDesc.byteSize = sizeof(g_Vertices);
		vertexBufferDesc.isVertexBuffer = true;
		vertexBufferDesc.debugName = "VertexBuffer";
		vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
		m_VertexBuffer = GetDevice()->createBuffer(vertexBufferDesc);

		m_CommandList->beginTrackingBufferState(m_VertexBuffer,
			nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));
		m_CommandList->setPermanentBufferState(m_VertexBuffer,
			nvrhi::ResourceStates::VertexBuffer);//note:will call end tracking buffer state

		nvrhi::BufferDesc indexBufferDesc;
		indexBufferDesc.byteSize = sizeof(g_Indices);
		indexBufferDesc.isIndexBuffer = true;
		indexBufferDesc.debugName = "IndexBuffer";
		indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
		m_IndexBuffer = GetDevice()->createBuffer(indexBufferDesc);

		m_CommandList->beginTrackingBufferState(m_IndexBuffer, nvrhi::ResourceStates::CopyDest);
		m_CommandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));
		m_CommandList->setPermanentBufferState(m_IndexBuffer,
			nvrhi::ResourceStates::IndexBuffer);

		//todo:load texture
		GuGuUtf8Str textureFileName = u8"asset/nvidia-logo.png";
		std::shared_ptr <LoadedTexture> texture = textureCache.LoadTextureFromFile(
			textureFileName, true, nullptr, m_CommandList);
		m_Texture = texture->texture;

		m_CommandList->close();
		//todo:add execute command list
		GetDevice()->executeCommandList(m_CommandList);

		// Create a single binding layout and multiple binding sets, one set per view.
		// The different binding sets use different slices of the same constant buffer.
		for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex) {
			nvrhi::BindingSetDesc bindingSetDesc;
			bindingSetDesc.bindings = {
				// Note: using viewIndex to construct a buffer range.
				nvrhi::BindingSetItem::ConstantBuffer(0, m_ConstantBuffer,
													  nvrhi::BufferRange(
															  sizeof(ConstantBufferEntry) *
															  viewIndex,
															  sizeof(ConstantBufferEntry))),
				// Texutre and sampler are the same for all model views.
				nvrhi::BindingSetItem::Texture_SRV(0, m_Texture),
				nvrhi::BindingSetItem::Sampler(0, commonPasses.m_AnisotropicWrapSampler)
			};

			// Create the binding layout (if it's empty -- so, on the first iteration) and the binding set.
			if (!nvrhi::utils::CreateBindingSetAndLayout(GetDevice(), nvrhi::ShaderType::All, 0,
				bindingSetDesc, m_BindingLayout,
				m_BindingSets[viewIndex])) {
				GuGu_LOGD("Couldn't create the binding set or layout");
				return false;
			}
		}

		return true;
	}

	void VertexBuffer::Update(float fElapsedTimeSeconds){
		//GuGu_LOGD("%f", seconds);
		m_Rotation += fElapsedTimeSeconds;
	}

	void VertexBuffer::BackBufferResizing(){
		m_Pipeline = nullptr;
	}

	void VertexBuffer::Render(nvrhi::IFramebuffer* framebuffer){
		const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();
		math::uint2 size = math::uint2(fbinfo.width, fbinfo.height);
		if (!m_renderTarget || math::any(m_renderTargetSize != size))
		{
			m_renderTarget = nullptr;

			m_renderTargetSize = size;

			initRenderTargetAndDepthTarget();
		}

		if (!m_Pipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_VertexShader;
			psoDesc.PS = m_PixelShader;
			psoDesc.inputLayout = m_InputLayout;
			psoDesc.bindingLayouts = { m_BindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;

			m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, m_frameBuffer);
		}

		m_CommandList->open();

		nvrhi::utils::ClearColorAttachment(m_CommandList, m_frameBuffer, 0, Color(0.2f));

		// Fill out the constant buffer slices for multiple views of the model.
		ConstantBufferEntry modelConstants[c_NumViews];
		for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex) {
			math::affine3 viewMatrix =
				math::rotation(normalize(g_RotationAxes[viewIndex]), m_Rotation)
				* math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
				* math::translation(math::float3(0, 0, 2));
			math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(60.f),
				float(fbinfo.width) /
				float(fbinfo.height), 0.1f,
				10.f);
			math::float4x4 viewProjMatrix = math::affineToHomogeneous(viewMatrix) * projMatrix;
			modelConstants[viewIndex].viewProjMatrix = viewProjMatrix;
		}

		// Upload all constant buffer slices at once.
		m_CommandList->writeBuffer(m_ConstantBuffer, modelConstants, sizeof(modelConstants));

		for (uint32_t viewIndex = 0; viewIndex < c_NumViews; ++viewIndex) {
			nvrhi::GraphicsState state;
			// Pick the right binding set for this view.
			state.bindings = { m_BindingSets[viewIndex] };
			state.indexBuffer = { m_IndexBuffer, nvrhi::Format::R32_UINT, 0 };
			// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
			state.vertexBuffers = {
					{m_VertexBuffer, 1, offsetof(Vertex, uv)},
					{m_VertexBuffer, 0, offsetof(Vertex, position)}
			};
			state.pipeline = m_Pipeline;
			state.framebuffer = m_frameBuffer;

			// Construct the viewport so that all viewports form a grid.
			const float width = float(fbinfo.width) * 0.5f;
			const float height = float(fbinfo.height) * 0.5f;
			const float left = width * float(viewIndex % 2);
			const float top = height * float(viewIndex / 2);

			const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top,
				top + height, 0.f, 1.f);
			state.viewport.addViewportAndScissorRect(viewport);

			// Update the pipeline, bindings, and other state.
			m_CommandList->setGraphicsState(state);

			// Draw the model.
			nvrhi::DrawArguments args;
			args.vertexCount = dim(g_Indices);
			m_CommandList->drawIndexed(args);
		}

		//if (!m_Pipeline)
		//{
		//    nvrhi::GraphicsPipelineDesc psoDesc;
		//    psoDesc.VS = m_VertexShader;
		//    psoDesc.PS = m_PixelShader;
		//    psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		//    psoDesc.renderState.depthStencilState.depthTestEnable = false;
		//
		//    m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		//}
		//
		//m_CommandList->open();
		//
		//nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(0.f));
		//
		//nvrhi::GraphicsState state;
		//state.pipeline = m_Pipeline;
		//state.framebuffer = framebuffer;
		//state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());
		//
		//m_CommandList->setGraphicsState(state);
		//
		//nvrhi::DrawArguments args;
		//args.vertexCount = 3;
		//m_CommandList->draw(args);
		//
		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);
	}
	nvrhi::TextureHandle VertexBuffer::getRenderTarget()
	{
		return m_renderTarget;
	}
	void VertexBuffer::initRenderTargetAndDepthTarget()
	{
		nvrhi::TextureDesc desc;
		desc.width = m_renderTargetSize.x;
		desc.height = m_renderTargetSize.y;
		desc.initialState = nvrhi::ResourceStates::RenderTarget;
		desc.isRenderTarget = true;
		desc.useClearValue = true;
		desc.clearValue = Color(0.f);
		desc.sampleCount = 1;
		desc.dimension = nvrhi::TextureDimension::Texture2D;
		desc.keepInitialState = true;
		desc.isTypeless = false;
		desc.isUAV = false;
		desc.mipLevels = 1;

		desc.format = nvrhi::Format::SRGBA8_UNORM;
		desc.debugName = "RenderTarget";
		m_renderTarget = GetDevice()->createTexture(desc);

		const nvrhi::Format depthFormats[] = {
			nvrhi::Format::D24S8,
			nvrhi::Format::D32S8,
			nvrhi::Format::D32,
			nvrhi::Format::D16 };

		const nvrhi::FormatSupport depthFeatures =
			nvrhi::FormatSupport::Texture |
			nvrhi::FormatSupport::DepthStencil |
			nvrhi::FormatSupport::ShaderLoad;

		desc.format = nvrhi::utils::ChooseFormat(GetDevice(), depthFeatures, depthFormats, std::size(depthFormats));
		desc.isTypeless = true;
		desc.initialState = nvrhi::ResourceStates::DepthWrite;
		desc.clearValue = Color(0.f);
		desc.debugName = "Depth";
		m_depthTarget = GetDevice()->createTexture(desc);

		nvrhi::FramebufferDesc fbDesc;
		fbDesc.addColorAttachment(m_renderTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));
		fbDesc.setDepthAttachment(m_depthTarget, nvrhi::TextureSubresourceSet(0, 1, 0, 1));

		m_frameBuffer = GetDevice()->createFramebuffer(fbDesc);
	}
}
