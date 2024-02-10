#include <pch.h>

#include "UIRenderPass.h"

#include <Renderer/TextureCache.h>
#include <Renderer/ShaderFactory.h>
#include <Renderer/utils.h>

#include "Brush.h"
#include "Style.h"
#include "BasicElement.h"

#include <Core/GuGuFile.h>

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
		m_textureCache = std::make_shared<TextureCache>(GetDevice());
		loadStyleTextures();

		auto samplerDesc = nvrhi::SamplerDesc()
			.setAllFilters(true)
			.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
		m_linearWrapSampler = GetDevice()->createSampler(samplerDesc);

		std::shared_ptr <ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>(
			GetDevice());
		m_vertexShader = shaderFactory->CreateShader("UIShader_main_vs.bin", "main_vs", nullptr,
			nvrhi::ShaderType::Vertex);
		m_pixelShader = shaderFactory->CreateShader("UIShader_main_ps.bin", "main_ps", nullptr,
			nvrhi::ShaderType::Pixel);

		if (!m_vertexShader || !m_pixelShader)
			return false;

		m_constantBuffer = GetDevice()->createBuffer(
			nvrhi::utils::CreateStaticConstantBufferDesc(
				sizeof(ConstantBufferEntry), "ConstantBuffer").setInitialState(nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(true));

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
			.setBufferIndex(0)
			.setElementStride(sizeof(UIVertex)),
			nvrhi::VertexAttributeDesc()
			.setName("COLOR0")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(0)
			.setBufferIndex(0)
			.setElementStride(sizeof(UIVertex)),
			nvrhi::VertexAttributeDesc()
			.setName("COLOR1")
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setOffset(0)
			.setBufferIndex(0)
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

		//nvrhi::BindingSetDesc bindingSetDesc;
		//bindingSetDesc.bindings = {
		//	// Note: using viewIndex to construct a buffer range.
		//	nvrhi::BindingSetItem::ConstantBuffer(0, m_constantBuffer,
		//										  nvrhi::BufferRange(
		//												  sizeof(ConstantBufferEntry),
		//												  sizeof(ConstantBufferEntry))),
		//	// Texutre and sampler are the same for all model views.
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

		GetDevice()->executeCommandList(m_CommandList);
		return true;
	}
	void UIRenderPass::Render(nvrhi::IFramebuffer* framebuffer)
	{
	}
	void UIRenderPass::Animate(float fElapsedTimeSeconds)
	{
	}
	void UIRenderPass::BackBufferResizing()
	{
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
			finalizeTexture(texture, m_CommandList.Get());
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
		m_CommandList->writeTexture(m_textureAtlas, 0, 0, dataPointer, m_atlasSize, 1);
		m_CommandList->setPermanentTextureState(m_textureAtlas, nvrhi::ResourceStates::ShaderResource);//todo:fix this
		m_CommandList->commitBarriers();
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
#if 1
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
		auto start = m_textureAtlasData.begin() + slotToCopyTo->y * m_atlasSize * 4 + slotToCopyTo->x * m_atlasSize * 4;

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
#if 1
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
#if 1
			copyRow(copyRowData);
#else
			zeroRow(copyRowData);
#endif
		}
	}
	void UIRenderPass::finalizeTexture(std::shared_ptr<TextureData> texture, nvrhi::ICommandList* commandList)
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
					rightSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, findSlot->y, padding);
				}
				else
				{
					leftSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, paddedHeight, padding);
					rightSlot = std::make_shared<AtlasedTextureSlot>(findSlot->x, findSlot->y + paddedHeight, findSlot->width, remainingHeight, paddedHeight);
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
	}
}