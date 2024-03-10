#include <pch.h>

#include "FontCache.h"

#include "TextInfo.h"
#include "CharacterList.h"
#include "FreeTypeFace.h"
#include "FreeTypeUtils.h"

#include "AtlasTexture.h"

#include <Core/Math/MyMath.h>

namespace GuGu {
	FontCache::FontCache()
	{
		m_atlasSize = 1024;
		m_atlasTexture = std::make_shared<AtlasTexture>(m_atlasSize, 1);
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
	void FontCache::Init(std::shared_ptr<FileSystem> fileSystem)
	{
		m_fileSystem = fileSystem;
	}
	math::float2 FontCache::measureText(const GuGuUtf8Str& str, const TextInfo& textInfo, float inScale)
	{
		std::shared_ptr<CharacterList> characterList = getCharacterList(textInfo, inScale);

		const uint16_t maxHeight = characterList->getMaxHeight();

		//the size of the string
		math::float2 size(0, 0);
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
	std::shared_ptr<AtlasedTextureSlot> FontCache::addCharacter(uint16_t inSizeX, uint16_t inSizeY, std::vector<uint8_t>& rawPixels)
	{
		return m_atlasTexture->loadAtlasSlots(inSizeX, inSizeY, rawPixels);
	}
	std::shared_ptr<CharacterList> FontCache::getCharacterList(const TextInfo& textInfo, float inScale)
	{
		FontKey fontKey(textInfo, inScale);
		if (m_characterLists.find(fontKey) != m_characterLists.end())
			return m_characterLists[fontKey];
		std::shared_ptr<CharacterList> characterList = std::make_shared<CharacterList>(fontKey);
		m_characterLists.insert({ fontKey, characterList });
		return characterList;
	}
	uint16_t FontCache::getMaxCharacterHeight(const TextInfo& textInfo, float scale)
	{
		GuGuUtf8Str Char(u8"0");

		std::shared_ptr<FreeTypeFace> freeTypeFace;
		if (m_faces.find(textInfo) != m_faces.end())
			freeTypeFace = m_faces[textInfo];
		else
		{
			GuGuUtf8Str filePath = textInfo.getFilePath();
			freeTypeFace = std::make_shared<FreeTypeFace>(&m_library, filePath, m_fileSystem);
			m_faces.insert({ textInfo, freeTypeFace });
		}    

		FT_Error error = FT_Get_Char_Index(freeTypeFace->getFontFace(), Char.getUnicode().at(0));
		//uint32_t glyphIndex = FT_Get_Char_Index(freeTypeFace->getFontFace(), Char.getUnicode().at(0));//todo:fix this
		FreeTypeUtils::ApplySizeAndScale(freeTypeFace->getFontFace(), textInfo.getSize(), scale);

		FT_Pos maxHeight = FT_MulFix(freeTypeFace->getFontFace()->height, freeTypeFace->getFontFace()->size->metrics.y_scale);

		return ((maxHeight + (1 << 5)) >> 6);
	}
	std::shared_ptr<FreeTypeFace> FontCache::getFreeTypeFace(const TextInfo& textInfo)
	{
		return m_faces[textInfo];
	}
	void FontCache::setFontAtlasTexture(nvrhi::TextureHandle newTexture)
	{
		m_atlasTexture->setTextureAtlas(newTexture);
	}
	nvrhi::TextureHandle FontCache::getFontAtlasTexture()
	{
		return m_atlasTexture->getTextureAtlas();
	}
	std::vector<uint8_t>& FontCache::getAtlasRawData()
	{
		return m_atlasTexture->getAtlasData();
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
		m_atlasTexture->setTextureAtlas(nullptr);
	}
}