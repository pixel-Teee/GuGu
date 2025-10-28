#pragma once

#include <list>
#include <vector>

#include <Renderer/nvrhi.h>

namespace GuGu {
	struct FCopyRowData
	{
		//source data to copy
		const uint8_t* srcData;
		//place to copy data to
		uint8_t* destData;
		//the row number to copy
		uint32_t srcRow;
		//the row number to copy to
		uint32_t destRow;
		//the width of a source row
		uint32_t rowWidth;
		//the width of the source texture
		uint32_t srcTextureWidth;
		//the width of the dest texture
		uint32_t destTextureWidth;
	};

	struct AtlasedTextureSlot
	{
		uint32_t x;
		uint32_t y;
		uint32_t width;
		uint32_t height;
		uint32_t padding;
		AtlasedTextureSlot(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inPadding = 1)
			: x(inX)
			, y(inY)
			, width(inWidth)
			, height(inHeight)
			, padding(inPadding)
		{}
	};

	class Brush;
	struct TextureData;
	class AtlasTexture {
	public:
		AtlasTexture(uint32_t atlasSize, uint32_t stride);

		virtual ~AtlasTexture();

		void setTextureAtlas(nvrhi::TextureHandle textureAtlas);

		nvrhi::TextureHandle getTextureAtlas();

		std::shared_ptr<AtlasedTextureSlot> loadAtlasSlots(uint32_t inWidth, uint32_t inHeight, const std::vector<uint8_t>& data);

		void copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data);

		std::vector<uint8_t>& getAtlasData();

		uint32_t getStride() const;
	private:
		uint32_t m_atlasSize;
		uint32_t m_stride;
		std::list<std::shared_ptr<AtlasedTextureSlot>> m_textureAtlasEmptySlots;
		std::list<std::shared_ptr<AtlasedTextureSlot>> m_textureAtlasUsedSlots;
		std::vector<uint8_t> m_textureAtlasData;

		nvrhi::TextureHandle m_textureAtlas;
		uint32_t m_slotPadding;
	};
}