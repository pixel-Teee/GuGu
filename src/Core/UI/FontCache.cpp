#include <pch.h>

#include "FontCache.h"

#include "CharacterList.h"
#include "TextInfo.h"
#include "FreeTypeFace.h"
#include "FreeTypeUtils.h"

#include <Core/Math/MyMath.h>
#include "UIRenderPass.h"//todo:fix this

namespace GuGu {
	FontCache::FontCache()
	{
		m_atlasSize = 1024;
		std::shared_ptr<AtlasedTextureSlot> rootSlot = std::make_shared<AtlasedTextureSlot>(0, 0, m_atlasSize, m_atlasSize, 1);
		//auto it = m_textureAtlasSlots.begin();
		m_fontAtlasTextureEmptySlots.push_front(rootSlot);
		m_atlasData.resize(m_atlasSize * m_atlasSize * 1, 0);
		m_fontAtlasDirty = false;
		FT_Init_FreeType(&m_library);
	}
	FontCache::~FontCache()
	{
		m_faces.clear();//note:this will call FT_Done_Face before FT_Done_FreeType
		FT_Done_FreeType(m_library);
	}
	std::shared_ptr<FontCache> FontCache::getFontCache()
	{
		static std::shared_ptr<FontCache> fontCache = std::make_shared<FontCache>();
		return fontCache;
	}
	math::double2 FontCache::measureText(const GuGuUtf8Str& str, const TextInfo& textInfo)
	{
		std::shared_ptr<CharacterList> characterList = getCharacterList(textInfo);

		const uint16_t maxHeight = characterList->getMaxHeight();

		//the size of the string
		math::double2 size(0, 0);
		//widest line encountered while drawing this text
		int32_t maxLineWidth = 0;
		//the width of the current line so far
		int32_t currentX = 0;
		//accumulate height of this block of text
		int32_t stringSizeY = maxHeight;
		//character from last frame
		GuGuUtf8Str previousChar(u8"\0");

		int32_t finalPosX = 0;
		int32_t charIndex;
		GuGuUtf8Str newLineChar(u8"\n");
		for (charIndex = 0; charIndex < str.len(); ++charIndex)
		{
			GuGuUtf8Str currentChar = str[charIndex];

			const bool isNewLine = (currentChar == newLineChar);

			if (isNewLine)
			{
				//we accumulate total height
				stringSizeY += maxHeight;
				//update the longest line we've encountered
				maxLineWidth = std::max(currentX, maxLineWidth);
				//the next line starts at the beginning
				currentX = 0;
			}
			else
			{
				std::shared_ptr<GlyphEntry> entry = characterList->getCharacter(currentChar);

				previousChar = currentChar;

				const int32_t totalCharSpacing = entry->m_glyphFontAtlasData.horizontalOffset + entry->xAdvance;

				currentX += entry->xAdvance;
			}
		}

		maxLineWidth = std::max(maxLineWidth, currentX);

		size.x = maxLineWidth;
		size.y = stringSizeY;

		return size;
	}
	std::shared_ptr<CharacterList> FontCache::getCharacterList(const TextInfo& textInfo)
	{
		if (m_characterLists.find(textInfo) != m_characterLists.end())
			return m_characterLists[textInfo];
		std::shared_ptr<CharacterList> characterList = std::make_shared<CharacterList>(textInfo);
		m_characterLists.insert({ textInfo, characterList });
		return characterList;
	}

	uint16_t FontCache::getMaxCharacterHeight(const TextInfo& textInfo)
	{
		GuGuUtf8Str Char(u8"0");

		std::shared_ptr<FreeTypeFace> freeTypeFace;
		if (m_faces.find(textInfo) != m_faces.end())
			freeTypeFace = m_faces[textInfo];
		else
		{
			GuGuUtf8Str filePath = textInfo.getFilePath();
			freeTypeFace = std::make_shared<FreeTypeFace>(&m_library, filePath);
			m_faces.insert({ textInfo, freeTypeFace });
		}    

		FT_Error error = FT_Get_Char_Index(freeTypeFace->getFontFace(), Char.getUnicode().at(0));
		//uint32_t glyphIndex = FT_Get_Char_Index(freeTypeFace->getFontFace(), Char.getUnicode().at(0));//todo:fix this
		FreeTypeUtils::ApplySizeAndScale(freeTypeFace->getFontFace(), textInfo.getSize(), 1.0);

		FT_Pos maxHeight = FT_MulFix(freeTypeFace->getFontFace()->height, freeTypeFace->getFontFace()->size->metrics.y_scale);

		return ((maxHeight + (1 << 5)) >> 6);
	}

	std::shared_ptr<FreeTypeFace> FontCache::getFreeTypeFace(const TextInfo& textInfo)
	{
		return m_faces[textInfo];
	}

	std::shared_ptr<AtlasedTextureSlot> FontCache::addCharacter(uint16_t inSizeX, uint16_t inSizeY, std::vector<uint8_t>& rawPixels)
	{
		//copy data into slot
		const uint32_t width = inSizeX;
		const uint32_t height = inSizeY;

		//account for padding on both sides
		const uint8_t padding = 1;
		const uint32_t totalPadding = padding * 2;
		const uint32_t paddedWidth = width + totalPadding;
		const uint32_t paddedHeight = height + totalPadding;

		std::shared_ptr<AtlasedTextureSlot> findSlot = nullptr;
		for (auto it : m_fontAtlasTextureEmptySlots)
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
				auto slotIt = std::find(m_fontAtlasTextureEmptySlots.begin(), m_fontAtlasTextureEmptySlots.end(), findSlot);
				m_fontAtlasTextureEmptySlots.erase(slotIt);
				auto slotIt2 = m_fontAtlasTextureEmptySlots.insert(m_fontAtlasTextureEmptySlots.begin(), leftSlot);
				++slotIt2;
				m_fontAtlasTextureEmptySlots.insert(slotIt2, rightSlot);//insert left and right, delete find slot

				m_fontAtlasTextureUsedSlots.push_back(findSlot);
			}
			else
			{
				auto slotIt = std::find(m_fontAtlasTextureEmptySlots.begin(), m_fontAtlasTextureEmptySlots.end(), findSlot);
				m_fontAtlasTextureEmptySlots.erase(slotIt);

				m_fontAtlasTextureUsedSlots.push_back(findSlot);
			}

			//shrink the slot the remaining area
			findSlot->width = paddedWidth;
			findSlot->height = paddedHeight;
		}


		const std::shared_ptr<AtlasedTextureSlot> newSlot = findSlot;

		if (newSlot && width > 0 && height > 0)
		{
			//copy data into slot
			copyDataIntoSlot(newSlot, rawPixels);
		}

		return newSlot;
	}

	nvrhi::TextureHandle FontCache::getFontAtlasTexture()
	{
		return m_fontAtlasTexture;
	}

	std::vector<uint8_t>& FontCache::getAtlasRawData()
	{
		return m_atlasData;
	}

	void FontCache::setFontAtlasTexture(nvrhi::TextureHandle newTexture)
	{
		m_fontAtlasTexture = newTexture;
	}

	bool FontCache::getDirtyFlag()
	{
		return m_fontAtlasDirty;
	}

	void FontCache::setDirtyFlag(bool value)
	{
		m_fontAtlasDirty = value;
	}

	void FontCache::clear()
	{
		m_fontAtlasTexture = nullptr;
	}

	void FontCache::copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data)
	{
		//copy pixel data to the texture
		auto start = m_atlasData.begin() + slotToCopyTo->y * m_atlasSize * 1 + slotToCopyTo->x * 1;

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

	void FontCache::copyRow(const FCopyRowData& copyRowData)
	{
		const uint8_t* data = copyRowData.srcData;
		uint8_t* start = copyRowData.destData;
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t srcRow = copyRowData.srcRow;
		const uint32_t destRow = copyRowData.destRow;

		const uint32_t padding = 1;
		const uint8_t* sourceDataAddr = &data[srcRow * sourceWidth * 1];
		uint8_t* destDataAddr = &start[(destRow * destWidth + padding) * 1];
		memcpy(destDataAddr, sourceDataAddr, sourceWidth * 1);

		if (padding > 0)
		{
			uint8_t* destPaddingPixelLeft = &start[destRow * destWidth * 1];
			uint8_t* destPaddingPixelRight = destPaddingPixelLeft + ((copyRowData.rowWidth - 1) * 1);
#if 0
			const uint8_t* firstPixel = sourceDataAddr;
			const uint8_t* lastPixel = sourceDataAddr + ((sourceWidth - 1) * 1);
			memcpy(destPaddingPixelLeft, firstPixel, 1);
			memcpy(destPaddingPixelRight, lastPixel, 1);
#else
			memset(destPaddingPixelLeft, 0, 1);
			memset(destPaddingPixelRight, 0, 1);
#endif		
		}
	}

	void FontCache::zeroRow(const FCopyRowData& copyRowData)
	{
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t destRow = copyRowData.destRow;

		uint8_t* destDataAddr = &copyRowData.destData[destRow * destWidth * 1];
		memset(destDataAddr, 0, copyRowData.rowWidth * 1);
	}

}