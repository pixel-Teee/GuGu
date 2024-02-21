#include <pch.h>

#include "UIRenderPass.h"

#include <Renderer/utils.h>
#include <Renderer/TextureCache.h>
#include <Renderer/ShaderFactory.h>

#include "Slot.h"
#include "Brush.h"
#include "Style.h"
#include "BasicElement.h"
#include "ElementList.h"

#include <Core/GuGuFile.h>

#include "ImageWidget.h"
#include "WindowWidget.h"
#include "TextBlockWidget.h"
#include "FontCache.h"

#include <Application/Application.h>

namespace GuGu {

	static std::vector <uint8_t> ReadTextureFile(const GuGuUtf8Str& path){
		std::vector<uint8_t> fileContent;
		std::shared_ptr<GuGuFile> file = CreateFileFactory();
		file->OpenFile(path, GuGuFile::FileMode::OnlyRead);
		int32_t fileLength = file->getFileSize();
		fileContent.resize(fileLength);
		int32_t haveReadedLength = 0;
		file->ReadFile(fileContent.data(), fileLength, haveReadedLength);
		file->CloseFile();
		return fileContent;
	}

	UIRenderPass::~UIRenderPass()
	{

	}
	bool UIRenderPass::Init()
	{
		m_CommandList = GetDevice()->createCommandList();
		m_CommandList->open();
		m_textureCache = std::make_shared<TextureCache>(GetDevice());
		m_styles = Style::getStyle();
		initAtlasData();
		loadStyleTextures();

		auto samplerDesc = nvrhi::SamplerDesc()
			.setAllFilters(false)
			.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
		m_pointWrapSampler = GetDevice()->createSampler(samplerDesc);

		std::shared_ptr <ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>(
			GetDevice());
		m_vertexShader = shaderFactory->CreateShader("UIShader_main_vs.bin", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_pixelShader = shaderFactory->CreateShader("UIShader_main_ps.bin", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);

		if (!m_vertexShader || !m_pixelShader)
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
		//nvrhi::BindingSetDesc bindingSetDesc;
		//bindingSetDesc.bindings = {
		//	// Note: using viewIndex to construct a buffer range.
		//	nvrhi::BindingSetItem::ConstantBuffer(0, m_constantBuffer,
		//										  nvrhi::BufferRange(
		//												  sizeof(ConstantBufferEntry),
		//												  sizeof(ConstantBufferEntry))),
		//	// Texture and sampler are the same for all model views.
		//	nvrhi::BindingSetItem::Texture_SRV(0, m_Texture),
		//	nvrhi::BindingSetItem::Sampler(0, commonPasses.m_AnisotropicWrapSampler)
		//};
		//
		//if (!nvrhi::utils::CreateBindingSetAndLayout(GetDevice(), nvrhi::ShaderType::All, 0,
		//	bindingSetDesc, m_bindingLayout,
		//	m_bindingSet)) {
		//	GuGu_LOGD("Couldn't create the binding set or layout");
		//	return false;
		//}
		
		//build ui tree
		std::shared_ptr<Application> application = Application::getApplication();
		std::shared_ptr<Window> window = application->getWindow(0);
		m_uiRoot = std::make_shared<WindowWidget>();
		m_uiRoot->assocateWithNativeWindow(window);//native window
		m_elementList = std::make_shared<ElementList>();
		std::shared_ptr<ImageWidget> imageWidget = std::make_shared<ImageWidget>();
		m_textBlockWidget = std::make_shared<TextBlockWidget>();
		m_uiRoot->setChildWidget(m_textBlockWidget);
		m_uiRoot->getSlot(0)->setHorizontalAlignment(HorizontalAlignment::Center);
		m_uiRoot->getSlot(0)->setVerticalAlignment(VerticalAlignment::Center);
		m_uiRoot->getSlot(0)->setPadding(Padding(0.0f, 0.0f, 0.0f, 0.0f));
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

			//math::affineToHomogeneous(worldToView)
			ConstantBufferEntry modelConstant;
			modelConstant.viewProjMatrix = projMatrix * math::affineToHomogeneous(worldToView);

			m_CommandList->writeBuffer(m_constantBuffers[i], &modelConstant, sizeof(modelConstant));

			nvrhi::BindingSetDesc desc;
			//desc.bindings = {
			//	nvrhi::BindingSetItem::ConstantBuffer(0, m_constantBuffers[i]),
			//	nvrhi::BindingSetItem::Texture_SRV(0, m_elementList->getBatches()[i]->m_brush->m_texture),
			//	nvrhi::BindingSetItem::Sampler(0, m_pointWrapSampler)
			//};

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
			state.pipeline = m_pipeline;
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
	void UIRenderPass::Animate(float fElapsedTimeSeconds)
	{
		float mfps = Application::getApplication()->getmFps();
		GuGuUtf8Str mfpsStr = u8"mfps:%.3f";
		mfpsStr = strFormat(mfpsStr, mfps);
		m_textBlockWidget->setText(mfpsStr);

		calculateWidgetsFixedSize(m_uiRoot);

		m_elementList->clear();
		
		math::int2 windowWidthAndHeight = math::int2(GetDeviceManager()->getDeviceCreationParameters().backBufferWidth, GetDeviceManager()->getDeviceCreationParameters().backBufferHeight);
		
		WidgetGeometry geometry;
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
		m_pipeline = nullptr;
	}
	void UIRenderPass::BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount)
	{
	}
	void UIRenderPass::initAtlasData()
	{
		std::shared_ptr<AtlasedTextureSlot> rootSlot = std::make_shared<AtlasedTextureSlot>(0, 0, m_atlasSize, m_atlasSize, 1);
		//auto it = m_textureAtlasSlots.begin();
		m_textureAtlasEmptySlots.push_front(rootSlot);
		m_textureAtlasData.resize(m_atlasSize * m_atlasSize * 4, 0); //stride is 4
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

			std::vector<uint8_t> fileData = ReadTextureFile(texturePath);
			m_textureCache->FillTextureData(fileData, texture, texturePath, "");
			
			if (!brushs[i]->m_tiling)
			{
				std::shared_ptr<AtlasedTextureSlot> slot = loadAtlasSlots(texture, brushs[i]);

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
				textureDesc.format = nvrhi::Format::RGBA8_UNORM;//todo:fix this
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

		const char* dataPointer = reinterpret_cast<const char*>(static_cast<const uint8_t*>(m_textureAtlasData.data()));//use this memory to update gpu texture

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
		m_textureAtlas = GetDevice()->createTexture(textureDesc);
		m_CommandList->beginTrackingTextureState(m_textureAtlas, nvrhi::AllSubresources, nvrhi::ResourceStates::Common);
		m_CommandList->writeTexture(m_textureAtlas, 0, 0, dataPointer, m_atlasSize * 4, 1);
		m_CommandList->setPermanentTextureState(m_textureAtlas, nvrhi::ResourceStates::ShaderResource);//todo:fix this
		m_CommandList->commitBarriers();

		for (size_t i = 0; i < brushs.size(); ++i)
		{
			if (!brushs[i]->m_tiling)
			{
				brushs[i]->m_texture = m_textureAtlas;
			}		
		}
	}
	void UIRenderPass::copyRow(const FCopyRowData& copyRowData)
	{
		const uint8_t* data = copyRowData.srcData;
		uint8_t* start = copyRowData.destData;
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t srcRow = copyRowData.srcRow;
		const uint32_t destRow = copyRowData.destRow;
		
		const uint32_t padding = 1;
		const uint8_t* sourceDataAddr = &data[srcRow * sourceWidth * 4];
		uint8_t* destDataAddr = &start[(destRow * destWidth + padding) * 4];
		memcpy(destDataAddr, sourceDataAddr, sourceWidth * 4);

		if (padding > 0)
		{
			uint8_t* destPaddingPixelLeft = &start[destRow * destWidth * 4];
			uint8_t* destPaddingPixelRight = destPaddingPixelLeft + ((copyRowData.rowWidth - 1) * 4);
#if 0
			const uint8_t* firstPixel = sourceDataAddr;
			const uint8_t* lastPixel = sourceDataAddr + ((sourceWidth - 1) * 4);
			memcpy(destPaddingPixelLeft, firstPixel, 4);
			memcpy(destPaddingPixelRight, lastPixel, 4);
#else
			memset(destPaddingPixelLeft, 0, 4);
			memset(destPaddingPixelRight, 0, 4);
#endif		
		}
	}
	void UIRenderPass::zeroRow(const FCopyRowData& copyRowData)
	{
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t destRow = copyRowData.destRow;

		uint8_t* destDataAddr = &copyRowData.destData[destRow * destWidth * 4];
		memset(destDataAddr, 0, copyRowData.rowWidth * 4);
	}
	void UIRenderPass::copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data)
	{
		//copy pixel data to the texture
		auto start = m_textureAtlasData.begin() + slotToCopyTo->y * m_atlasSize * 4 + slotToCopyTo->x * 4;

		const uint32_t padding = 1;
		const uint32_t allPadding = padding * 2;

		//the width of the source texture without padding(actual width)
		const uint32_t sourceWidth = slotToCopyTo->width - allPadding;
		const uint32_t sourceHeight = slotToCopyTo->height - allPadding;

		FCopyRowData copyRowData;
		copyRowData.destData = &(*start);
		copyRowData.srcData = data.data();
		copyRowData.destTextureWidth = m_atlasSize;
		copyRowData.srcTextureWidth = sourceWidth;
		copyRowData.rowWidth = slotToCopyTo->width;

		if (padding > 0)
		{
			//copy first color row into padding
			copyRowData.srcRow = 0;
			copyRowData.destRow = 0;
#if 0
			copyRow(copyRowData);
#else
			zeroRow(copyRowData);
#endif
		}

		//copy each row of the texture
		for (uint32_t row = padding; row < slotToCopyTo->height - padding; ++row)
		{
			copyRowData.srcRow = row - padding;
			copyRowData.destRow = row;

			copyRow(copyRowData);
		}

		if (padding > 0)
		{
			//copy last color row into padding row for bilinear filtering
			copyRowData.srcRow = sourceHeight - 1;
			copyRowData.destRow = slotToCopyTo->height - padding;
#if 0
			copyRow(copyRowData);
#else
			zeroRow(copyRowData);
#endif
		}
	}
	std::shared_ptr<AtlasedTextureSlot> UIRenderPass::loadAtlasSlots(std::shared_ptr<TextureData> texture, std::shared_ptr<Brush> brush)
	{
		const uint32_t width = texture->width;
		const uint32_t height = texture->height;
		
		//account for padding on both sides
		const uint8_t padding = 1;
		const uint32_t totalPadding = padding * 2;
		const uint32_t paddedWidth = width + totalPadding;
		const uint32_t paddedHeight = height + totalPadding;
	
		std::shared_ptr<AtlasedTextureSlot> findSlot = nullptr;
		for (auto it : m_textureAtlasEmptySlots)
		{
			if (paddedWidth <= it->width && paddedHeight <= it->height)
			{
				findSlot = it;
				break;
			}
		}

		if (findSlot != nullptr)
		{
			//the width and height of the new child node
			const uint32_t remainingWidth = std::max(0u, findSlot->width - paddedWidth);
			const uint32_t remainingHeight = std::max(0u, findSlot->height - paddedHeight);

			const uint32_t minSlotDim = 2;

			//split the remaining area around this slot into two children
			if (remainingWidth >= minSlotDim || remainingHeight >= minSlotDim)
			{
				std::shared_ptr<AtlasedTextureSlot> leftSlot;
				std::shared_ptr<AtlasedTextureSlot> rightSlot;

				if (remainingHeight <= remainingWidth)
				{
					leftSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x, findSlot->y + paddedHeight, paddedWidth, remainingHeight, padding);
					rightSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, findSlot->height, padding);
				}
				else
				{
					leftSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, paddedHeight, padding);
					rightSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x, findSlot->y + paddedHeight, findSlot->width, remainingHeight, padding);
				}

				//replace the old slot within atlas empty slots, with the new left and right slot, then add the old slot to atlas used slots
				auto slotIt = std::find(m_textureAtlasEmptySlots.begin(), m_textureAtlasEmptySlots.end(), findSlot);
				m_textureAtlasEmptySlots.erase(slotIt);
				auto slotIt2 = m_textureAtlasEmptySlots.insert(m_textureAtlasEmptySlots.begin(), leftSlot);
				++slotIt2;
				m_textureAtlasEmptySlots.insert(slotIt2, rightSlot);//insert left and right, delete find slot

				m_textureAtlasUsedSlots.push_back(findSlot);
			}
			else
			{
				auto slotIt = std::find(m_textureAtlasEmptySlots.begin(), m_textureAtlasEmptySlots.end(), findSlot);
				m_textureAtlasEmptySlots.erase(slotIt);

				m_textureAtlasUsedSlots.push_back(findSlot);
			}

			//shrink the slot the remaining area
			findSlot->width = paddedWidth;
			findSlot->height = paddedHeight;
		}
		

		const std::shared_ptr<AtlasedTextureSlot> newSlot = findSlot;

		if (newSlot && width > 0 && height > 0)
		{
			//copy data into slot
			copyDataIntoSlot(newSlot, texture->data);
		}

		return newSlot;
	}
	void UIRenderPass::calculateWidgetsFixedSize(std::shared_ptr<Widget> widget)
	{
		uint32_t widgetNumbers = widget->getSlotsNumber();
		math::double2 size = math::double2(0.0, 0.0);
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<Slot> childSlot = widget->getSlot(i);
			if (childSlot)
			{
				std::shared_ptr<Widget> childWidget = childSlot->getChildWidget();
				calculateWidgetsFixedSize(childWidget);
				//size += childWidget->ComputeFixedSize();
			}
		}
		size = widget->ComputeFixedSize();
		widget->setFixedSize(size);
	}
	void UIRenderPass::generateWidgetElement(WidgetGeometry& allocatedWidgetGeometry)
	{
		m_uiRoot->GenerateElement(*m_elementList, allocatedWidgetGeometry, 0);
	}
}