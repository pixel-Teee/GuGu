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

#include "Graph.h"
#include "Border.h"
#include "Button.h"
#include "Overlay.h"
#include "BoxPanel.h"
#include "CheckBox.h"
#include "NullWidget.h"
#include "ImageWidget.h"
#include "WindowWidget.h"
#include "Viewport.h"
#include "TextBlockWidget.h"
#include "EditableText.h"
#include "EditableTextBox.h"
#include "Slider.h"
#include "AtlasTexture.h"
#include "FontCache.h"
#include "UIMacros.h"
#include "Attribute.h"
#include "SpinBox.h"
#include "ListPanel.h"
#include "ScrollBar.h"
#include "ListView.h"
#include "TableRow.h"
#include "Spacer.h"
#include "TreeView.h"
#include "ComboButton.h"
#include "Box.h"
#include "ComboBox.h"

#include <Core/GuGuFile.h>
#include <Window/Window.h>
#include <Application/Application.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/UI/UIData.h>
#include <Core/Timer.h>
#include <Renderer/CommonRenderPasses.h>
#include <Core/SceneGraph/SceneGraph.h>

namespace GuGu {

	UIRenderPass::~UIRenderPass()
	{

	}
	bool UIRenderPass::Init(std::shared_ptr<UIData> uiData)
	{
		m_uiData = uiData;

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

		m_commonRenderPass = std::make_shared<CommonRenderPasses>(GetDevice(), shaderFactory);

		std::vector<ShaderMacro> macros;
		macros.push_back(ShaderMacro("UI_Default", "1"));
		//macros.push_back(ShaderMacro("UI_Font", "1"));

		m_vertexShader = shaderFactory->CreateShader("asset/shader/UIShader.hlsl", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_pixelShader = shaderFactory->CreateShader("asset/shader/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		macros.clear();
		macros.push_back(ShaderMacro("UI_Font", "1"));
		m_pixelFontShader = shaderFactory->CreateShader("asset/shader/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		macros.clear();
		macros.push_back(ShaderMacro("UI_Line", "1"));
		m_lineShader = shaderFactory->CreateShader("asset/shader/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		macros.clear();
		macros.push_back(ShaderMacro("UI_RoundedBox", "1"));
		m_roundedBoxPixelShader = shaderFactory->CreateShader("asset/shader/UIShader.hlsl", "main_ps", &macros,
			nvrhi::ShaderType::Pixel);

		if (!m_vertexShader || !m_pixelShader || !m_pixelFontShader || !m_lineShader || !m_roundedBoxPixelShader)
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
		//WIDGET_ASSIGN_NEW(WindowWidget, m_uiRoot)
		//	.Type(WindowWidget::NativeWindow)
		//	.Content
		//	(
		//        WIDGET_NEW(HorizontalBox)
		//		+ HorizontalBox::Slot()
		//		.FixedWidth()
		//		(
		//			WIDGET_NEW(TextBlockWidget)
		//			.textLambda([&] {
		//				float mfps = Application::getApplication()->getmFps();
		//				uint32_t fps = Application::getApplication()->getFps();
		//				GuGuUtf8Str fpsStr = u8"帧率的倒数\n一帧耗费时长:%.3f毫秒\n帧率:%dfps";
		//				fpsStr = strFormat(fpsStr, mfps, fps);
		//				return fpsStr;
		//			})
		//		)
		//		+ HorizontalBox::Slot()
		//		.FixedWidth()
		//		(
		//			WIDGET_NEW(ImageWidget)
		//		)
		//		+ HorizontalBox::Slot()
		//		.StretchWidth(Attribute<float>(0.3))
		//		(
		//			WIDGET_NEW(ImageWidget)
		//			.brush(m_styles->getBrush("biscuit"))
		//		)
		//		+ HorizontalBox::Slot()
		//		.StretchWidth(Attribute<float>(0.6))
		//		(
		//			WIDGET_NEW(VerticalBox)
		//			+ VerticalBox::Slot()
		//			.StretchHeight(0.7)
		//			(
		//				WIDGET_NEW(ImageWidget)
		//			)
		//			+ VerticalBox::Slot()
		//			.StretchHeight(0.4)
		//			(
		//				WIDGET_NEW(ImageWidget)
		//				.brush(m_styles->getBrush("biscuit"))
		//			)
		//		)
		//	);

		createTestWindow();

		std::shared_ptr<Window> window = application->getWindow(0);
		//m_uiRoot = std::make_shared<WindowWidget>();
		m_uiRoot->assocateWithNativeWindow(window);//native window
		m_elementList = std::make_shared<ElementList>();
		//std::shared_ptr<ImageWidget> imageWidget = std::make_shared<ImageWidget>();
		//m_textBlockWidget = std::make_shared<TextBlockWidget>();
		//m_uiRoot->setChildWidget(m_textBlockWidget);
		//m_uiRoot->getSlot(0)->setHorizontalAlignment(HorizontalAlignment::Center);
		//m_uiRoot->getSlot(0)->setVerticalAlignment(VerticalAlignment::Center);
		//m_uiRoot->getSlot(0)->setPadding(Padding(0.0f, 0.0f, 0.0f, 0.0f));
		////m_textBlockWidget->ComputeFixedSize();
		//GuGuUtf8Str text = u8"愉悦送走";
		//m_textBlockWidget->setText(text);
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
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
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

		if (!m_LinePipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_vertexShader;
			psoDesc.PS = m_lineShader;
			psoDesc.inputLayout = m_inputLayout;
			psoDesc.bindingLayouts = { m_bindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			m_LinePipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		}

		if (!m_roundedBoxPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = m_vertexShader;
			psoDesc.PS = m_roundedBoxPixelShader;
			psoDesc.inputLayout = m_inputLayout;
			psoDesc.bindingLayouts = { m_bindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.blendState.targets[0].setBlendEnable(true);
			psoDesc.renderState.blendState.targets[0].setSrcBlend(nvrhi::BlendFactor::SrcAlpha);
			psoDesc.renderState.blendState.targets[0].setDestBlend(nvrhi::BlendFactor::OneMinusSrcAlpha);
			psoDesc.renderState.depthStencilState.depthTestEnable = false;
			m_roundedBoxPipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		}

		m_CommandList->open();

		math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
		math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
		math::float3 cameraUp = math::float3(0.0f, -1.0f, 0.0f);
		math::float3 cameraRight = normalize(cross(cameraDir, cameraUp));
		cameraUp = normalize(cross(cameraRight, cameraDir));
		math::affine3 worldToView = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
		worldToView = worldToView * math::rotation(math::normalize(math::float3(0.0f, 0.0f, 1.0f)), math::radians(Application::getApplication()->getGlobalPreRotate()));
        math::float4x4 projMatrix = math::orthoProjD3DStyle(0, fbinfo.width, 0, fbinfo.height, 0, 1);
        math::float4x4 vp = projMatrix * math::affineToHomogeneous(worldToView);
		for (size_t i = 0; i < m_IndexBuffers.size(); ++i)
		{
			ConstantBufferEntry modelConstant;
			modelConstant.viewProjMatrix = vp;
			modelConstant.shaderParam = m_elementList->getBatches()[i]->m_shaderParams.pixelParams;
			modelConstant.shaderParam2 = m_elementList->getBatches()[i]->m_shaderParams.pixelParams2;
			m_CommandList->writeBuffer(m_constantBuffers[i], &modelConstant, sizeof(modelConstant));

			nvrhi::TextureHandle batchTexture = m_elementList->getBatches()[i]->m_texture;
			if (batchTexture == nullptr)
				batchTexture = m_commonRenderPass->m_whiteTexture;
			else
				batchTexture = m_elementList->getBatches()[i]->m_texture;

			nvrhi::BindingSetDesc desc;
			desc.bindings = {
				nvrhi::BindingSetItem::ConstantBuffer(0, m_constantBuffers[i]),
				nvrhi::BindingSetItem::Texture_SRV(0, batchTexture),
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
			if (m_elementList->getBatches()[i]->shaderType == UIShaderType::Default)
				state.pipeline = m_pipeline;
			else if (m_elementList->getBatches()[i]->shaderType == UIShaderType::Font)
				state.pipeline = m_FontPipeline;
			else if (m_elementList->getBatches()[i]->shaderType == UIShaderType::Line)
				state.pipeline = m_LinePipeline;
			else
				state.pipeline = m_roundedBoxPipeline;
			state.framebuffer = framebuffer;

			//construct the viewport so that all viewports form a grid.
			const float width = float(fbinfo.width);
			const float height = float(fbinfo.height);

			const nvrhi::Viewport viewport = nvrhi::Viewport(0, width, 0, height, 0.f, 1.f);
			state.viewport.addViewport(viewport);
			if (m_elementList->getBatches()[i]->m_clippingState != nullptr)
			{
				ClippingZone clipZone = m_elementList->getBatches()[i]->m_clippingState->m_scissorRect.value();
                //math::affine2 globalPreRotate =  math::rotation(math::radians(Application::getApplication()->getGlobalPreRotate()));
                math::float2 points[2] = {clipZone.m_topLeft, clipZone.m_bottomRight};
                math::float2 pointsNew[2];
                for(uint32_t i = 0; i < 2; ++i)
                {
                    math::float4 tempPoint = math::float4(points[i].x, points[i].y, 0.0f, 1.0f) * vp;
					tempPoint = math::float4(tempPoint.x / tempPoint.w, tempPoint.y / tempPoint.w, tempPoint.z / tempPoint.w, tempPoint.w / tempPoint.w);
					pointsNew[i] = math::viewportMatrix(width, height) * tempPoint;//todo:fix this, this is platform specific viewport
                }
				math::float2 pointsNew2[2];
				pointsNew2[0].x = std::min(pointsNew[0].x, pointsNew[1].x);
				pointsNew2[1].x = std::max(pointsNew[0].x, pointsNew[1].x);
				pointsNew2[0].y = std::min(pointsNew[0].y, pointsNew[1].y);
				pointsNew2[1].y = std::max(pointsNew[0].y, pointsNew[1].y);

                nvrhi::Rect clipScissor(pointsNew2[0].x, pointsNew2[1].x, pointsNew2[0].y, pointsNew2[1].y);
				//nvrhi::Rect clipScissor(clipZone.m_topLeft.x, clipZone.m_topRight.x, clipZone.m_topRight.y, clipZone.m_bottomRight.y);
				state.viewport.addScissorRect(clipScissor);//todo:add global pre rotate
			}
			else
			{
				state.viewport.addScissorRect(nvrhi::Rect(viewport));
			}

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
		//m_listView->rebuildList();
		//float mfps = Application::getApplication()->getmFps();
		//uint32_t fps = Application::getApplication()->getFps();
		//GuGuUtf8Str mfpsStr = u8"帧率的倒数\n一帧耗费时长:%.3f毫秒\n帧率:%dfps";
		//mfpsStr = strFormat(mfpsStr, mfps, fps);
		//m_textBlockWidget->setText(mfpsStr);
		m_CommandList->open();
		updateTextAtlasTexture();//batch will reference the font texture

		calculateWidgetsFixedSize(m_uiRoot);

		m_elementList->clear();
		
		math::int2 windowWidthAndHeight = math::int2(GetDeviceManager()->getDeviceCreationParameters().backBufferWidth, GetDeviceManager()->getDeviceCreationParameters().backBufferHeight);
		
		//设置窗口大小
		m_uiRoot->setCachedSize(math::float2(windowWidthAndHeight.x, windowWidthAndHeight.y));

		WidgetGeometry geometry = WidgetGeometry::makeRoot(math::float2(windowWidthAndHeight.x / m_uiRoot->getNativeWindow()->getDpiFactor(), windowWidthAndHeight.y / m_uiRoot->getNativeWindow()->getDpiFactor()),
			math::affine2(math::float2x2::diagonal(m_uiRoot->getNativeWindow()->getDpiFactor()), 0.0f));
		//geometry.setAbsoluteScale(m_uiRoot->getNativeWindow()->getDpiFactor());
		//todo:fix this
		//geometry.setLocalSize(math::float2(windowWidthAndHeight.x / m_uiRoot->getNativeWindow()->getDpiFactor(), windowWidthAndHeight.y / m_uiRoot->getNativeWindow()->getDpiFactor()));
		generateWidgetElement(geometry);
		m_elementList->generateBatches();

		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		//generate vertex and index
		
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

	std::shared_ptr<WindowWidget> UIRenderPass::getWindowWidget()
	{
		return m_uiRoot;//todo:fix this
	}

	std::vector<std::shared_ptr<Widget>> UIRenderPass::getAllWidgets()
	{
		return m_allWidgets;
	}

	void UIRenderPass::setRenderTarget(nvrhi::TextureHandle renderTarget)
	{
		if(m_viewport)
			m_viewport->setRenderTarget(renderTarget);
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

			if(texturePath == "") continue;//no resource

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
	//自下而上，计算固定大小
	void UIRenderPass::calculateWidgetsFixedSize(std::shared_ptr<WindowWidget> windowWidget)
	{
		windowWidget->prepass(windowWidget->getNativeWindow()->getDpiFactor());
	}
	//自上而下递归，分配实际空间
	void UIRenderPass::generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry)
	{
		m_allWidgets.clear();
		PaintArgs paintArgs(m_allWidgets, Application::getApplication()->getTimer()->GetTotalTime(), Application::getApplication()->getTimer()->GetDeltaTime());

		math::box2 cullingRect(math::float2(0.0f, 0.0f), math::float2(allocatedWidgetGeometry.getLocalSize().x, allocatedWidgetGeometry.getLocalSize().y));
		m_uiRoot->generateElement(paintArgs, cullingRect, *m_elementList, allocatedWidgetGeometry, 0);
	}
	std::shared_ptr<WindowWidget> UIRenderPass::createTestWindow()
	{
		static std::vector<GuGuUtf8Str> strVec = { u8"test", u8"你好", u8"这是一个combo box", u8"这是一个combo button" };

		return WIDGET_ASSIGN_NEW(WindowWidget, m_uiRoot)
			.Type(WindowWidget::NativeWindow)
			.Content //fill and fill
			(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.StretchWidth(1.0f)
				(
					WIDGET_NEW(VerticalBox)
					+ VerticalBox::Slot()
					.FixedHeight()
					(
						WIDGET_NEW(Overlay)
						+ Overlay::Slot()
						.setHorizontalAlignment(HorizontalAlignment::Stretch)
						.setVerticalAlignment(VerticalAlignment::Stretch)
						(
							WIDGET_NEW(ImageWidget)
							//.Clip(WidgetClipping::ClipToBounds)
							.brush(m_styles->getBrush("headerBackground"))
						)
						+ Overlay::Slot()
						.setHorizontalAlignment(HorizontalAlignment::Right)
						.setVerticalAlignment(VerticalAlignment::Center)
						.setPadding(Padding(0.0f, 0.0f, 10.0f, 0.0f))
						(
							WIDGET_NEW(Button)
							.buttonSyle(m_styles->getStyle<ButtonStyle>("closeButton"))
							//.contentPadding(Padding(10.0f, 10.0f, 10.0f, 10.0f))
							//.Clip(WidgetClipping::ClipToBounds)
							.ClickedLambda([&] {
									static uint32_t i = 0;
									
									++i;
									if (i % 2)
									{
										m_textColor = math::float4(0.2f, 0.5f, 0.7f, 1.0f);
									}
									else
									{
										m_textColor = math::float4(0.9f, 0.5f, 0.7f, 1.0f);
									}
									return Reply::Unhandled();
								}
							)	
							.Content
							(
								NullWidget::getNullWidget()
							)
						)
					)
					+ VerticalBox::Slot()
					.StretchHeight(1.0f)
					(
						WIDGET_NEW(HorizontalBox)
						+ HorizontalBox::Slot()
						.StretchWidth(1.0f)
						(
							WIDGET_NEW(Border)
							.verticalAlignment(VerticalAlignment::Center)
							.horizontalAlignment(HorizontalAlignment::Center)
							.brush(m_styles->getBrush("background"))
							//.Clip(WidgetClipping::ClipToBounds)
							.Content
							(
								WIDGET_NEW(VerticalBox)
								+VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(TextBlockWidget)
									.Clip(WidgetClipping::ClipToBounds)
									.textLambda([&] {
										float mfps = Application::getApplication()->getmFps();
										uint32_t fps = Application::getApplication()->getFps();
										GuGuUtf8Str fpsStr = u8"一帧耗费时长:%.3f毫秒\r\n帧率:%dfps";
										fpsStr = strFormat(fpsStr, mfps, fps);
										//GuGuUtf8Str str = u8"臂跳释阔孕\r\n型佩层愧茎\r\n橘代克屿牛\r\n炊间成羞蝴\r\n顾堆殿友岁\r\n火柜九体妇\r\n枯窑雄密般\r\n鞭宵醉始著\r\n复赚霸扰夺\r\n况说柄响附\r\n庄存叔劫屈\r\n健疯窜滥限\r\n追侵删榆容\r\n挺左丢帮器\r\n搜涂酬魂很\r\n配你御字栗\r\n勇尚沫葡吗\r\n菊庙侍引镜\r\n笔络没蜓页\r\n灾穿卵奴绕";
										return fpsStr;
										})
									.textColorLambda([&] {
											math::float4 color = m_textColor;
											return color;
									})
								)
								+ VerticalBox::Slot()
								.StretchHeight(1.0f)
								(
									WIDGET_ASSIGN_NEW(ViewportWidget, m_viewport)
									.Content(
										WIDGET_NEW(Overlay)
										+Overlay::Slot()
										.setPadding(Padding(20.0f, 20.0f, 0.0f, 0.0f))
										.setHorizontalAlignment(HorizontalAlignment::Left)
										.setVerticalAlignment(VerticalAlignment::Top)
										(
											WIDGET_NEW(TextBlockWidget)
											//.Clip(WidgetClipping::ClipToBounds)
											.text("测试Overlay")
										)			
									)
								)
								//+ VerticalBox::Slot()
								//.FixedHeight()
								//(
								//	WIDGET_NEW(HorizontalBox)
								//	+ HorizontalBox::Slot()
								//	.FixedWidth()
								//	(
								//		WIDGET_NEW(TextBlockWidget)
								//		.text("复选框")
								//	)
								//	+ HorizontalBox::Slot()
								//	.FixedWidth()
								//	(
								//		WIDGET_NEW(CheckBox)
								//		.Content
								//		(
								//			WIDGET_NEW(NullWidget)
								//		)
								//	)					
								//	
								//)
								+ VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									.StretchWidth(0.2f)
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"金属度")
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Slider)
										.MaxValue(1.0f)
										.MinValue(0.0f)
										.OnValueChangedLambda([&](float inValue) {
												m_uiData->metallic = inValue;
											}
										)
									)
									//+ HorizontalBox::Slot()
									//.FixedWidth()
									//(
									//	WIDGET_NEW(TextBlockWidget)
									//	.text(u8"金属度")
									//	)
									//+ HorizontalBox::Slot()
									//.StretchWidth(1.0f)
									//(
									//	WIDGET_NEW(Slider)
									//	.MaxValue(1.0f)
									//	.MinValue(0.0f)
									//	.OnValueChangedLambda([&](float inValue) {
									//		m_uiData->metallic = inValue;
									//		}
									//	)
									//)
								)
								+ VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									.StretchWidth(0.2f)
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"粗糙度")
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Slider)
										.MaxValue(1.0f)
										.MinValue(0.0f)
										.OnValueChangedLambda([&](float inValue) {
												m_uiData->roughness = inValue;
											}
										)
									)
								)
								//+ VerticalBox::Slot()
								//.FixedHeight()
								//(
								//	WIDGET_NEW(HorizontalBox)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(0.2f)
								//	(
								//		WIDGET_NEW(TextBlockWidget)
								//		.text(u8"width")
								//	)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(1.0f)
								//	(
								//		WIDGET_NEW(Slider)
								//		.MaxValue(15.0f)
								//		.MinValue(0.0f)
								//		.OnValueChangedLambda([&](float inValue) {
								//				StyleSet::getStyle()->getBrush("thumbImage")->m_outlineSettings.m_width = inValue;
								//			}
								//		)
								//	)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(0.2f)
								//	(
								//		WIDGET_NEW(TextBlockWidget)
								//		.text(u8"cornerRadius")
								//		)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(1.0f)
								//	(
								//		WIDGET_NEW(Slider)
								//		.MaxValue(200.0f)
								//		.MinValue(0.0f)
								//		.OnValueChangedLambda([&](float inValue) {
								//			StyleSet::getStyle()->getBrush("thumbImage")->m_outlineSettings.m_cornerRadius = math::float4(inValue);
								//			}
								//		)
								//	)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(0.2f)
								//	(
								//		WIDGET_NEW(TextBlockWidget)
								//		.text(u8"Padding")
								//		)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(1.0f)
								//	(
								//		WIDGET_NEW(Slider)
								//		.MaxValue(15.0f)
								//		.MinValue(0.0f)
								//		.OnValueChangedLambda([&](float inValue) {
								//			StyleSet::getStyle()->getBrush("thumbImage")->m_margin = Padding(inValue, inValue, inValue, inValue);
								//			}
								//		)
								//	)
								//)
								+ VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									.StretchWidth(0.2f)
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"dir")
										)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Slider)
										.MaxValue(10.0f)
										.MinValue(-10.0f)
										.OnValueChangedLambda([&](float inValue) {
											m_uiData->dir = inValue;
											}
										)
									)
								)
								+ VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									.StretchWidth(0.2f)
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"pos")
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Slider)
										.MaxValue(0.0f)
										.MinValue(-40.0f)
										.OnValueChangedLambda([&](float inValue) {
											m_uiData->camPos = inValue;
											}
										)
									)
								)
								+ VerticalBox::Slot()
								.FixedHeight()
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									.StretchWidth(0.3f)
									(
										WIDGET_NEW(TextBlockWidget)
										.text(u8"可编辑文本框:")
										.Clip(WidgetClipping::ClipToBounds)
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(EditableTextBox)
										.Text("文本测试你")
									)
								)
								+ VerticalBox::Slot()
								.StretchHeight(0.1)
								(
									WIDGET_NEW(HorizontalBox)
									+ HorizontalBox::Slot()
									
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Border)
										.Content(
											WIDGET_NEW(TextBlockWidget)
											.text(u8"x:")
											.Clip(WidgetClipping::ClipToBounds)
										)
										.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
										.BorderBackgroundColor(math::float4(1.0f, 0.0f, 0.0f, 1.0f))
										.horizontalAlignment(HorizontalAlignment::Center)
										.verticalAlignment(VerticalAlignment::Center)
									)
									+ HorizontalBox::Slot()
									.FixedWidth()
									(
										WIDGET_NEW(SpinBox<double>)
										.onValueChangedLambda([&](float newValue) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newValue, newTranslation.y, newTranslation.z));
											}
										})
										.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newValue, newTranslation.y, newTranslation.z));
											}
										})
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										WIDGET_NEW(Border)
										.Content(
											WIDGET_NEW(TextBlockWidget)
											.text(u8"y:")
											.Clip(WidgetClipping::ClipToBounds)
										)
										.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
										.BorderBackgroundColor(math::float4(0.0f, 1.0f, 0.0f, 1.0f))
										.horizontalAlignment(HorizontalAlignment::Center)
										.verticalAlignment(VerticalAlignment::Center)
									)
									+ HorizontalBox::Slot()
									.FixedWidth()
									(
										WIDGET_NEW(SpinBox<double>)
										.onValueChangedLambda([&](float newValue) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newTranslation.x, newValue, newTranslation.z));
											}				
										})
										.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newTranslation.x, newValue, newTranslation.z));
											}
										})
									)
									+ HorizontalBox::Slot()
									.StretchWidth(1.0f)
									(
										
										WIDGET_NEW(Border)
										.Content(
											WIDGET_NEW(TextBlockWidget)
											.text(u8"z:")
											.Clip(WidgetClipping::ClipToBounds)
										)
										.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
										.BorderBackgroundColor(math::float4(0.0f, 0.0f, 1.0f, 1.0f))
										.horizontalAlignment(HorizontalAlignment::Center)
										.verticalAlignment(VerticalAlignment::Center)
									)
									+ HorizontalBox::Slot()
									.FixedWidth()
									(
										WIDGET_NEW(SpinBox<double>)
										.onValueChangedLambda([&](float newValue) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newTranslation.x, newTranslation.y, newValue));
											}
										})
										.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
											if (m_uiData->m_currentSelectItems) {
												math::double3 newTranslation = m_uiData->m_currentSelectItems->getTranslation();
												m_uiData->m_currentSelectItems->SetTranslation(math::double3(newTranslation.x, newTranslation.y, newValue));
											}
										})
									)
								)
								//+ VerticalBox::Slot()
								//.StretchHeight(0.1)
								//(
								//	WIDGET_NEW(HorizontalBox)
								//	+ HorizontalBox::Slot()
								//	
								//	.StretchWidth(1.0f)
								//	(
								//		WIDGET_NEW(Border)
								//		.Content(
								//			WIDGET_NEW(TextBlockWidget)
								//			.text(u8"R:")
								//			.Clip(WidgetClipping::ClipToBounds)
								//		)
								//		.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
								//		.BorderBackgroundColor(math::float4(1.0f, 0.0f, 0.0f, 1.0f))
								//	)
								//	+ HorizontalBox::Slot()
								//	.FixedWidth()
								//	(
								//		WIDGET_NEW(SpinBox<double>)
								//		.onValueChangedLambda([&](float newValue) {
								//			m_uiData->color.x = newValue;
								//		})
								//		.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
								//			m_uiData->color.x = newValue;
								//		})
								//	)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(1.0f)
								//	(
								//		WIDGET_NEW(Border)
								//		.Content(
								//			WIDGET_NEW(TextBlockWidget)
								//			.text(u8"G:")
								//			.Clip(WidgetClipping::ClipToBounds)
								//		)
								//		.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
								//		.BorderBackgroundColor(math::float4(0.0f, 1.0f, 0.0f, 1.0f))
								//	)
								//	+ HorizontalBox::Slot()
								//	.FixedWidth()
								//	(
								//		WIDGET_NEW(SpinBox<double>)
								//		.onValueChangedLambda([&](float newValue) {
								//			m_uiData->color.y = newValue;
								//		})
								//		.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
								//			m_uiData->color.y = newValue;
								//		})
								//	)
								//	+ HorizontalBox::Slot()
								//	.StretchWidth(1.0f)
								//	(
								//		
								//		WIDGET_NEW(Border)
								//		.Content(
								//			WIDGET_NEW(TextBlockWidget)
								//			.text(u8"B:")
								//			.Clip(WidgetClipping::ClipToBounds)
								//		)
								//		.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
								//		.BorderBackgroundColor(math::float4(0.0f, 0.0f, 1.0f, 1.0f))
								//	)
								//	+ HorizontalBox::Slot()
								//	.FixedWidth()
								//	(
								//		WIDGET_NEW(SpinBox<double>)
								//		.onValueChangedLambda([&](float newValue) {
								//			m_uiData->color.z = newValue;
								//		})
								//		.onValueCommittedLambda([&](float newValue, TextCommit::Type) {
								//			m_uiData->color.z = newValue;
								//		})
								//	)
								//)
							)
						)
						//+ HorizontalBox::Slot()
						//.StretchWidth(1.0f)
						//(
						//	WIDGET_NEW(Border)
						//	.verticalAlignment(VerticalAlignment::Stretch)
						//	.horizontalAlignment(HorizontalAlignment::Stretch)
						//	.brush(m_styles->getBrush("background"))
						//	.Content
						//	(
						//		WIDGET_NEW(Slider)
						//		.MaxValue(1.0f)
						//		.MinValue(0.0f)
						//		.orientation(Orientation::Vertical)
						//		.OnValueChangedLambda([&](float inValue) {
						//			m_uiData->metallic = inValue;
						//		}
						//		)
						//	)					
						//)
						//+HorizontalBox::Slot()
						//.StretchWidth(1.0f)
						//(
						//	WIDGET_NEW(Border)
						//	.verticalAlignment(VerticalAlignment::Stretch)
						//	.horizontalAlignment(HorizontalAlignment::Stretch)
						//	.brush(m_styles->getBrush("background"))
						//	.Content
						//	(
						//		WIDGET_NEW(ListPanel)
						//		.ListOrientation(Orientation::Horizontal)
						//		//.NumDesiredItems(6)
						//		.ItemWidth(400)
						//		.ItemHeight(120)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(Border)
						//			.verticalAlignment(VerticalAlignment::Stretch)
						//			.horizontalAlignment(HorizontalAlignment::Stretch)
						//			.brush(m_styles->getBrush("headerBackground"))
						//			.BorderBackgroundColor(math::float4(0.7f, 0.3f, 0.1f, 1.0f))
						//			.Content(
						//				WIDGET_NEW(TextBlockWidget)
						//				.text(u8"你好")
						//			)
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好我")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好我")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好我")
						//		)
						//		+ ListPanel::Slot()
						//		(
						//			WIDGET_NEW(TextBlockWidget)
						//			.text(u8"你好我")
						//		)
						//	)
						//)
						//+ HorizontalBox::Slot()
						//.StretchWidth(0.1f)
						//(
						//	WIDGET_NEW(ScrollBar)
						//)
						+ HorizontalBox::Slot()
						.StretchWidth(1.0f)
						(
							WIDGET_NEW(VerticalBox)
							+ VerticalBox::Slot()
							.StretchHeight(0.8f)
							(
								WIDGET_NEW(Border)
								.verticalAlignment(VerticalAlignment::Stretch)
								.horizontalAlignment(HorizontalAlignment::Stretch)
								.brush(m_styles->getBrush("background"))
								.Content
								(
									WIDGET_ASSIGN_NEW(TreeView<GuGuUtf8Str>, m_listView)
									.treeItemSource(&m_uiData->nodeNames)
									.onGetChildren(m_uiData->getNodeChildrens)
									.onGenerateRowLambda([](GuGuUtf8Str item, const std::shared_ptr<class TableViewBase>& table)->std::shared_ptr<ITableRow> {
										//static int32_t i = 0;
										//++i;
										math::float4 color = math::float4(0.3f, 0.6f, 0.2f, 1.0f);
										return WIDGET_NEW(TableRow<GuGuUtf8Str>, table)
											.Content
											(
												WIDGET_NEW(Border)
												.Content(
													WIDGET_NEW(TextBlockWidget)
													.text(item)
												)
												.brush(StyleSet::getStyle()->getStyle<SliderStyle>("slider")->m_normalThumbImage)
												.BorderBackgroundColor(color)
											);
										})
									.onSelectionChanged(m_uiData->selectionChanged)
								)
								.BorderBackgroundColor(math::float4(1.0f, 1.0f, 1.0f, 0.8f))
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9)
							(
								WIDGET_NEW(TextBlockWidget)
								.text(this, &UIRenderPass::getSelectItem)
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9)
							(
								WIDGET_NEW(Spacer)
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9)
							(
								//WIDGET_NEW(ComboButton)
								//.onGetMenuContentLambda([this]() {
								//	return WIDGET_NEW(BoxWidget)
								//	.HeightOverride(80.0f)
								//	.Content(
								//		WIDGET_NEW(ListView<GuGuUtf8Str>)
								//		.ListItemSource(&strVec)
								//		.onGenerateRowLambda([](GuGuUtf8Str item, const std::shared_ptr<class TableViewBase>& table)->std::shared_ptr<ITableRow> {
								//			return WIDGET_NEW(TableRow<GuGuUtf8Str>, table)
								//				.Content(
								//					WIDGET_NEW(Border)
								//					.BorderBackgroundColor(math::float4(0.3f, 0.6f, 0.2f, 1.0f))
								//					.Content
								//					(
								//						WIDGET_NEW(TextBlockWidget)
								//						.text(item)
								//					)
								//				);
								//			})	
								//	);
								//})
								//.buttonContent
								//(
								//	WIDGET_NEW(TextBlockWidget)
								//	.text(u8"combobutton测试")
								//)
								WIDGET_ASSIGN_NEW(ComboBox<GuGuUtf8Str>, m_comboBox)
								.optionSource(&strVec)
								.onGenerateWidgetLambda([&](GuGuUtf8Str inOption)->std::shared_ptr<Widget> {
									return WIDGET_NEW(Border)
										.BorderBackgroundColor(math::float4(0.3f, 0.6f, 0.2f, 1.0f))
										.Content
										(
											WIDGET_NEW(TextBlockWidget)
											.text(inOption)
										);
								})
								.initiallySelectedItem(u8"test")
								.Content
								(
									WIDGET_NEW(TextBlockWidget)
									.textLambda([&]()->GuGuUtf8Str {
										return m_comboBox->getSelectedItem();
									})
								)
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9f)
							(
								WIDGET_NEW(HorizontalBox)
								+ HorizontalBox::Slot()
								.StretchWidth(0.35f)
								(
									WIDGET_NEW(TextBlockWidget)
									.text(u8"axisLineWidth:")
								)
								.setVerticalAlignment(VerticalAlignment::Center)
								+ HorizontalBox::Slot()
								.StretchWidth(1.0f)
								(
									WIDGET_NEW(Slider)
									.MaxValue(20.0f)
									.MinValue(0.0f)
									.OnValueChangedLambda([&](float inValue) {
										m_uiData->m_gridProperties._axisLineWidth = inValue;
										}
									)
								)
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9f)
							(
								WIDGET_NEW(HorizontalBox)
								+ HorizontalBox::Slot()
								.StretchWidth(0.35f)
								(
									WIDGET_NEW(TextBlockWidget)
									.text(u8"MajorLineWidth:")
								)
								.setVerticalAlignment(VerticalAlignment::Center)
								+ HorizontalBox::Slot()
								.StretchWidth(1.0f)
								(
									WIDGET_NEW(Slider)
									.MaxValue(20.0f)
									.MinValue(0.0f)
									.OnValueChangedLambda([&](float inValue) {
										m_uiData->m_gridProperties._majorLineWidth = inValue;
										}
									)
								)
							)
							+ VerticalBox::Slot()
							.StretchHeight(0.9f)
							(
								WIDGET_NEW(HorizontalBox)
								+ HorizontalBox::Slot()
								.StretchWidth(0.35f)
								(
									WIDGET_NEW(TextBlockWidget)
									.text(u8"MinorLineWidth:")
								)
								.setVerticalAlignment(VerticalAlignment::Center)
								+ HorizontalBox::Slot()
								.StretchWidth(1.0f)
								(
									WIDGET_NEW(Slider)
									.MaxValue(20.0f)
									.MinValue(0.0f)
									.OnValueChangedLambda([&](float inValue) {
										m_uiData->m_gridProperties._minorLineWidth = inValue;
										}
									)
								)
							)
						)
					)
				)
				//+ HorizontalBox::Slot()
				//.StretchWidth(1.0)
				//(
				//	WIDGET_NEW(Slider)
				//	.orientation(Orientation::Vertical)
				//)
			);
	}
	GuGuUtf8Str UIRenderPass::getSelectItem() const
	{
		GuGuUtf8Str selectItem;
		if (m_uiData->m_currentSelectItems)
			selectItem = u8"选中了:" + m_uiData->m_currentSelectItems->GetName();

		return selectItem;
	}
}