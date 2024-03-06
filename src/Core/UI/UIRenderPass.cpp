#include <pch.h>

#include "UIRenderPass.h"

#include <Renderer/utils.h>
#include <Renderer/TextureCache.h>
#include <Renderer/ShaderFactory.h>

#include "Slot.h"
#include "Brush.h"
#include "StyleSet.h"
#include "BasicElement.h"
#include "ElementList.h"

#include "ImageWidget.h"
#include "WindowWidget.h"
#include "TextBlockWidget.h"
#include "AtlasTexture.h"
#include "FontCache.h"
#include "UIMacros.h"
#include "Attribute.h"

#include <Core/GuGuFile.h>
#include <Window/Window.h>
#include <Application/Application.h>
#include <Core/FileSystem/FileSystem.h>

namespace GuGu {

	UIRenderPass::~UIRenderPass()
	{

	}
	bool UIRenderPass::Init()
	{
		m_CommandList = GetDevice()->createCommandList();
		m_CommandList->open();
		GuGuUtf8Str assetPath = Application::GetDirectoryWithExecutable();
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
		m_textureCache = std::make_shared<TextureCache>(GetDevice(), m_rootFileSystem);
		m_styles = StyleSet::getStyle();

		m_atlasTexture = std::make_shared<AtlasTexture>(m_atlasSize, 4);
		loadStyleTextures();

		std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		fontCache->Init(m_rootFileSystem);

		auto samplerDesc = nvrhi::SamplerDesc()
			.setAllFilters(false)
			.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
		m_pointWrapSampler = GetDevice()->createSampler(samplerDesc);

		std::shared_ptr <ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>(
			GetDevice(), m_rootFileSystem);

		std::vector<ShaderMacro> macros;
		macros.push_back(ShaderMacro("UI_Default", "1"));
		//macros.push_back(ShaderMacro("UI_Font", "1"));

		m_vertexShader = shaderFactory->CreateShader("asset/UIShader.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_pixelShader = shaderFactory->CreateShader("asset/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		macros.clear();
		macros.push_back(ShaderMacro("UI_Font", "1"));
		m_pixelFontShader = shaderFactory->CreateShader("asset/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		if (!m_vertexShader || !m_pixelShader || !m_pixelFontShader)
			return false;

		nvrhi::VertexAttributeDesc attributes[] = {
			nvrhi::VertexAttributeDesc()
			.setName("TEXCOORD0")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(0)
			.setBufferIndex(0)
			.setElementStride(sizeof(UIVertex)),
			nvrhi::VertexAttributeDesc()
			.setName("POSITION0")
			.setFormat(nvrhi::Format::RG32_FLOAT)
			.setOffset(0)
			.setBufferIndex(1)
			.setElementStride(sizeof(UIVertex)),
			nvrhi::VertexAttributeDesc()
			.setName("COLOR0")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(0)
			.setBufferIndex(2)
			.setElementStride(sizeof(UIVertex)),
			nvrhi::VertexAttributeDesc()
			.setName("COLOR1")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(0)
			.setBufferIndex(3)
			.setElementStride(sizeof(UIVertex))
		};
		m_inputLayout = GetDevice()->createInputLayout(attributes, uint32_t(std::size(attributes)), m_vertexShader);

		nvrhi::BindingLayoutDesc layoutDesc;
		layoutDesc.visibility = nvrhi::ShaderType::All;
		layoutDesc.bindings = {
			nvrhi::BindingLayoutItem::ConstantBuffer(0),
			nvrhi::BindingLayoutItem::Texture_SRV(0),
			nvrhi::BindingLayoutItem::Sampler(0)
		};

		m_bindingLayout = GetDevice()->createBindingLayout(layoutDesc);
		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);

		//build ui tree
		std::shared_ptr<Application> application = Application::getApplication();
		WIDGET_ASSIGN_NEW(WindowWidget, m_uiRoot)
			.Type(WindowWidget::VirtualWindow)
			.Content
			(
				WIDGET_NEW(TextBlockWidget)
				.textLambda([&] {
					float mfps = Application::getApplication()->getmFps();
					uint32_t fps = Application::getApplication()->getFps();
					GuGuUtf8Str fpsStr = u8"帧率的倒数\n一帧耗费时长:%.3f毫秒\n帧率:%dfps";
					fpsStr = strFormat(fpsStr, mfps, fps);
					return fpsStr;
				})
			);
		std::shared_ptr<Window> window = application->getWindow(0);
		//m_uiRoot = std::make_shared<WindowWidget>();
		m_uiRoot->assocateWithNativeWindow(window);//native window
		m_elementList = std::make_shared<ElementList>();
		//std::shared_ptr<ImageWidget> imageWidget = std::make_shared<ImageWidget>();
		//m_textBlockWidget = std::make_shared<TextBlockWidget>();
		//m_uiRoot->setChildWidget(m_textBlockWidget);
		//m_uiRoot->getSlot(0)->setHorizontalAlignment(HorizontalAlignment::Left);
		//m_uiRoot->getSlot(0)->setVerticalAlignment(VerticalAlignment::Top);
		//m_uiRoot->getSlot(0)->setPadding(Padding(0.0f, 120.0f, 0.0f, 0.0f));
		//textBlockWidget->ComputeFixedSize();
		return true;
	}
	void UIRenderPass::Render(nvrhi::IFramebuffer* framebuffer)
	{
		const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();
		if (!m_pipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_vertexShader;
			psoDesc.PS = m_pixelShader;
			psoDesc.inputLayout = m_inputLayout;
			psoDesc.bindingLayouts = { m_bindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			//psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;//todo:fix this

			m_pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		}

		if (!m_FontPipeline) {
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_vertexShader;
			psoDesc.PS = m_pixelFontShader;
			psoDesc.inputLayout = m_inputLayout;
			psoDesc.bindingLayouts = { m_bindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.depthStencilState.depthTestEnable = false;

			m_FontPipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		}

		m_CommandList->open();

		math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
		math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
		math::float3 cameraUp = math::float3(0.0f, -1.0f, 0.0f);
		math::float3 cameraRight = normalize(cross(cameraDir, cameraUp));
		cameraUp = normalize(cross(cameraRight, cameraDir));
		
		math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
		worldToView = translation(-cameraPos) * worldToView;

		for (size_t i = 0; i < m_IndexBuffers.size(); ++i)
		{
			math::float4x4 projMatrix = math::orthoProjD3DStyle(0, fbinfo.width, 0, fbinfo.height, 0, 1);

			ConstantBufferEntry modelConstant;
			modelConstant.viewProjMatrix = projMatrix * math::affineToHomogeneous(worldToView);

			m_CommandList->writeBuffer(m_constantBuffers[i], &modelConstant, sizeof(modelConstant));

			nvrhi::BindingSetDesc desc;
			desc.bindings = {
				nvrhi::BindingSetItem::ConstantBuffer(0, m_constantBuffers[i]),
				nvrhi::BindingSetItem::Texture_SRV(0, m_elementList->getBatches()[i]->m_texture),
				nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
			};

			nvrhi::BindingSetHandle bindingSet;
			bindingSet = GetDevice()->createBindingSet(desc, m_bindingLayout);

			nvrhi::GraphicsState state;
			// Pick the right binding set for this view.
			state.bindings = { bindingSet };
			state.indexBuffer = { m_IndexBuffers[i], nvrhi::Format::R32_UINT, 0};
			// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
			state.vertexBuffers = {
				{ m_VertexBuffers[i], 1, offsetof(UIVertex, position)},
				{ m_VertexBuffers[i], 0, offsetof(UIVertex, textureCoordinate)},
				{ m_VertexBuffers[i], 2, offsetof(UIVertex, color)},
				{ m_VertexBuffers[i], 3, offsetof(UIVertex, secondaryColor)}
			};
			if(m_elementList->getBatches()[i]->shaderType == UIShaderType::Default)
				state.pipeline = m_pipeline;
			else if(m_elementList->getBatches()[i]->shaderType == UIShaderType::Font)
				state.pipeline = m_FontPipeline;
			state.framebuffer = framebuffer;

			// Construct the viewport so that all viewports form a grid.
			const float width = float(fbinfo.width);
			const float height = float(fbinfo.height);

			const nvrhi::Viewport viewport = nvrhi::Viewport(0, width, 0, height, 0.f, 1.f);
			state.viewport.addViewportAndScissorRect(viewport);

			// Update the pipeline, bindings, and other state.
			m_CommandList->setGraphicsState(state);

			// Draw the model.
			nvrhi::DrawArguments args;
			args.vertexCount = m_elementList->getBatches()[i]->m_indices.size();
			m_CommandList->drawIndexed(args);
		}
		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);
	}
	void UIRenderPass::Update(float fElapsedTimeSeconds)
	{
		//float mfps = Application::getApplication()->getmFps();
		//uint32_t fps = Application::getApplication()->getFps();
		//GuGuUtf8Str mfpsStr = u8"帧率的倒数\n一帧耗费时长:%.3f毫秒\n帧率:%dfps";
		//mfpsStr = strFormat(mfpsStr, mfps, fps);
		//m_textBlockWidget->setText(mfpsStr);

		calculateWidgetsFixedSize(m_uiRoot);

		m_elementList->clear();
		
		math::int2 windowWidthAndHeight = math::int2(GetDeviceManager()->getDeviceCreationParameters().backBufferWidth, GetDeviceManager()->getDeviceCreationParameters().backBufferHeight);
		
		WidgetGeometry geometry;
		geometry.setAbsoluteScale(m_uiRoot->getNativeWindow()->getDpiFactor());
		geometry.setLocalSize(math::double2(windowWidthAndHeight.x, windowWidthAndHeight.y));
		generateWidgetElement(geometry);
		m_elementList->generateBatches();
		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		//generate vertex and index
		m_CommandList->open();
		m_constantBuffers.clear();
		std::vector<std::shared_ptr<BatchData>> batches = m_elementList->getBatches();
		for (size_t i = 0; i < batches.size(); ++i)
		{
			nvrhi::BufferDesc vertexBufferDesc;
			vertexBufferDesc.byteSize = sizeof(UIVertex) * batches[i]->m_vertices.size();
			vertexBufferDesc.isVertexBuffer = true;
			vertexBufferDesc.debugName = "UIVertexBuffer";
			vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
			m_VertexBuffers.push_back(GetDevice()->createBuffer(vertexBufferDesc));

			m_CommandList->beginTrackingBufferState(m_VertexBuffers[i],
				nvrhi::ResourceStates::CopyDest);
			m_CommandList->writeBuffer(m_VertexBuffers[i], batches[i]->m_vertices.data(), sizeof(UIVertex) * batches[i]->m_vertices.size());
			m_CommandList->setPermanentBufferState(m_VertexBuffers[i],
				nvrhi::ResourceStates::VertexBuffer);//note:will call end tracking buffer state

			nvrhi::BufferDesc indexBufferDesc;
			indexBufferDesc.byteSize = sizeof(uint32_t) * batches[i]->m_indices.size();
			indexBufferDesc.isIndexBuffer = true;
			indexBufferDesc.debugName = "IndexBuffer";
			indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
			m_IndexBuffers.push_back(GetDevice()->createBuffer(indexBufferDesc));

			m_CommandList->beginTrackingBufferState(m_IndexBuffers[i], nvrhi::ResourceStates::CopyDest);
			m_CommandList->writeBuffer(m_IndexBuffers[i], batches[i]->m_indices.data(), sizeof(uint32_t) * batches[i]->m_indices.size());
			m_CommandList->setPermanentBufferState(m_IndexBuffers[i],
				nvrhi::ResourceStates::IndexBuffer);

			m_constantBuffers.push_back(GetDevice()->createBuffer(
				nvrhi::utils::CreateStaticConstantBufferDesc(
					sizeof(ConstantBufferEntry), "ConstantBuffer").setInitialState(nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(true)));
		}

		updateTextAtlasTexture();

		m_CommandList->close();
		GetDevice()->executeCommandList(m_CommandList);
	}
	void UIRenderPass::BackBufferResizing()
	{
		m_pipeline = m_FontPipeline = nullptr;
	}
	void UIRenderPass::BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount)
	{
	}

	void UIRenderPass::updateTextAtlasTexture()
	{
		std::shared_ptr<FontCache> fontCache = FontCache::getFontCache();
		nvrhi::TextureHandle texture = fontCache->getFontAtlasTexture();
		if (texture == nullptr || fontCache->getDirtyFlag())
		{
			fontCache->setDirtyFlag(false);
			const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(fontCache->getAtlasRawData().data()));
			nvrhi::TextureDesc textureDesc;
			textureDesc.format = nvrhi::Format::R8_UNORM;//todo:fix this
			textureDesc.width = 1024;
			textureDesc.height = 1024;
			textureDesc.depth = 1;
			textureDesc.arraySize = 1;
			textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
			textureDesc.mipLevels = 1;
			textureDesc.debugName = "font-atlas";//todo:fix this
			textureDesc.isRenderTarget = true;
			texture = GetDevice()->createTexture(textureDesc);
			m_CommandList->beginTrackingTextureState(texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
			m_CommandList->writeTexture(texture, 0, 0, dataPointer, 1024, 1);
			m_CommandList->setPermanentTextureState(texture, nvrhi::ResourceStates::ShaderResource);//todo:fix this
			m_CommandList->commitBarriers();
			fontCache->setFontAtlasTexture(texture);
		}
	}
	void UIRenderPass::loadStyleTextures()
	{
		std::vector<std::shared_ptr<Brush>> brushs;
		m_styles->getBrush(brushs);

		for (size_t i = 0; i < brushs.size(); ++i)
		{
			const GuGuUtf8Str& texturePath = brushs[i]->m_texturePath;

			std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();
			texture->path = texturePath;

			std::vector<uint8_t> fileData = ReadFile(texturePath, m_rootFileSystem);
			m_textureCache->FillTextureData(fileData, texture, texturePath, "");
			
			if (!brushs[i]->m_tiling)
			{
				std::shared_ptr<AtlasedTextureSlot> slot = m_atlasTexture->loadAtlasSlots(texture->width, texture->height, texture->data);	

				brushs[i]->m_startUV = math::double2((float)(slot->x + 1) / m_atlasSize, (float)(slot->y + 1) / m_atlasSize);
				brushs[i]->m_sizeUV = math::double2((float)(slot->width - 2) / m_atlasSize, (float)(slot->height - 2) / m_atlasSize);
				brushs[i]->m_actualSize = math::int2(texture->width, texture->height);
			}
			else
			{
				brushs[i]->m_startUV = math::double2(0.0f, 0.0f);
				brushs[i]->m_sizeUV = math::double2(1.0f, 1.0f);
				brushs[i]->m_actualSize = math::int2(texture->width, texture->height);

				const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(texture->data.data()));
				nvrhi::TextureDesc textureDesc;
				textureDesc.format = texture->format;
				textureDesc.width = brushs[i]->m_actualSize.x;
				textureDesc.height = brushs[i]->m_actualSize.y;
				textureDesc.depth = 1;
				textureDesc.arraySize = 1;
				textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
				textureDesc.mipLevels = 1;
				textureDesc.debugName = brushs[i]->m_texturePath;//todo:fix this
				textureDesc.isRenderTarget = true;
				brushs[i]->m_texture = GetDevice()->createTexture(textureDesc);
				m_CommandList->beginTrackingTextureState(brushs[i]->m_texture, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
				m_CommandList->writeTexture(brushs[i]->m_texture, 0, 0, dataPointer, brushs[i]->m_actualSize.x * 4, 1);
				m_CommandList->setPermanentTextureState(brushs[i]->m_texture, nvrhi::ResourceStates::ShaderResource);//todo:fix this
				m_CommandList->commitBarriers();
			}
		}

		const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(m_atlasTexture->getAtlasData().data()));//use this memory to update gpu texture

		nvrhi::TextureDesc textureDesc;
		textureDesc.format = nvrhi::Format::RGBA8_UNORM;//todo:fix this
		textureDesc.width = m_atlasSize;
		textureDesc.height = m_atlasSize;
		textureDesc.depth = 1;
		textureDesc.arraySize = 1;
		textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
		textureDesc.mipLevels = 1;
		textureDesc.debugName = "UITextureAtlas";
		textureDesc.isRenderTarget = true;
		nvrhi::TextureHandle textureAtlas = GetDevice()->createTexture(textureDesc);
		m_CommandList->beginTrackingTextureState(textureAtlas, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
		m_CommandList->writeTexture(textureAtlas, 0, 0, dataPointer, m_atlasSize * m_atlasTexture->getStride(), 1);
		m_CommandList->setPermanentTextureState(textureAtlas, nvrhi::ResourceStates::ShaderResource);//todo:fix this
		m_CommandList->commitBarriers();

		for (size_t i = 0; i < brushs.size(); ++i)
		{
			if (!brushs[i]->m_tiling)
			{
				brushs[i]->m_texture = textureAtlas;
			}		
		}
		m_atlasTexture->setTextureAtlas(textureAtlas);
	}
	void UIRenderPass::calculateWidgetsFixedSize(std::shared_ptr<WindowWidget> windowWidget)
	{
		windowWidget->prepass(windowWidget->getNativeWindow()->getDpiFactor());
	}
	void UIRenderPass::generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry)
	{
		m_uiRoot->GenerateElement(*m_elementList, allocatedWidgetGeometry, 0);
	}
}