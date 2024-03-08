#include <pch.h>

#include "AtlasTexture.h"

#include <Renderer/TextureCache.h>

namespace GuGu {
	void copyRow(const FCopyRowData& copyRowData, uint32_t stride)
	{
		const uint8_t* data = copyRowData.srcData;
		uint8_t* start = copyRowData.destData;
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t srcRow = copyRowData.srcRow;
		const uint32_t destRow = copyRowData.destRow;

		const uint32_t padding = 1;
		const uint8_t* sourceDataAddr = &data[srcRow * sourceWidth * stride];
		uint8_t* destDataAddr = &start[(destRow * destWidth + padding) * stride];
		memcpy(destDataAddr, sourceDataAddr, sourceWidth * stride);

		if (padding > 0)
		{
			uint8_t* destPaddingPixelLeft = &start[destRow * destWidth * stride];
			uint8_t* destPaddingPixelRight = destPaddingPixelLeft + ((copyRowData.rowWidth - 1) * stride);
#if 1
			const uint8_t* firstPixel = sourceDataAddr;
			const uint8_t* lastPixel = sourceDataAddr + ((sourceWidth - 1) * stride);
			memcpy(destPaddingPixelLeft, firstPixel, stride);
			memcpy(destPaddingPixelRight, lastPixel, stride);
#else
			memset(destPaddingPixelLeft, 0, stride);
			memset(destPaddingPixelRight, 0, stride);
#endif		
		}
	}
	void zeroRow(const FCopyRowData& copyRowData, uint32_t stride)
	{
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t destRow = copyRowData.destRow;

		uint8_t* destDataAddr = &copyRowData.destData[destRow * destWidth * stride];
		memset(destDataAddr, 0, copyRowData.rowWidth * stride);
	}
	AtlasTexture::AtlasTexture(uint32_t atlasSize, uint32_t stride)
		: m_atlasSize(atlasSize)
		, m_stride(stride)
	{
		std::shared_ptr<AtlasedTextureSlot> rootSlot = std::make_shared<AtlasedTextureSlot>(0, 0, m_atlasSize, m_atlasSize, 1);
		m_textureAtlasEmptySlots.push_front(rootSlot);
		m_textureAtlasData.resize(m_atlasSize * m_atlasSize * m_stride, 0);//stride
	}
	AtlasTexture::~AtlasTexture()
	{
	}
	void AtlasTexture::setTextureAtlas(nvrhi::TextureHandle textureAtlas)
	{
		m_textureAtlas = textureAtlas;
	}
	nvrhi::TextureHandle AtlasTexture::getTextureAtlas()
	{
		return m_textureAtlas;
	}
	std::shared_ptr<AtlasedTextureSlot> AtlasTexture::loadAtlasSlots(uint32_t inWidth, uint32_t inHeight, const std::vector<uint8_t>& data)
	{
		const uint32_t width = inWidth;
		const uint32_t height = inHeight;

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
			copyDataIntoSlot(newSlot, data);
		}

		return newSlot;
	}
	void AtlasTexture::copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data)
	{
		//copy pixel data to the texture
		auto start = m_textureAtlasData.begin() + slotToCopyTo->y * m_atlasSize * m_stride + slotToCopyTo->x * m_stride;

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
			copyRow(copyRowData, m_stride);
#else
			zeroRow(copyRowData, m_stride);
#endif
		}

		//copy each row of the texture
		for (uint32_t row = padding; row < slotToCopyTo->height - padding; ++row)
		{
			copyRowData.srcRow = row - padding;
			copyRowData.destRow = row;

			copyRow(copyRowData, m_stride);
		}

		if (padding > 0)
		{
			//copy last color row into padding row for bilinear filtering
			copyRowData.srcRow = sourceHeight - 1;
			copyRowData.destRow = slotToCopyTo->height - padding;
#if 1
			copyRow(copyRowData, m_stride);
#else
			zeroRow(copyRowData, m_stride);
#endif
		}
	}
	std::vector<uint8_t>& AtlasTexture::getAtlasData()
	{
		return m_textureAtlasData;
	}
	uint32_t AtlasTexture::getStride() const
	{
		return m_stride;
	}
}