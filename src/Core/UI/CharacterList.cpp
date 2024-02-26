#include <pch.h>

#include "CharacterList.h"

#include "FontCache.h"
#include "FreeTypeFace.h"
#include <freetype/ftadvanc.h>
#include "UIRenderPass.h"//todo:fix this

namespace GuGu {
	CharacterList::CharacterList(const FontKey& fontKey)
		: m_fontKey(fontKey)
		, m_MaxHeight(0)
	{
	}
	CharacterList::~CharacterList()
	{
	}
	uint16_t CharacterList::getMaxHeight()
	{
		if (m_MaxHeight == 0)
		{
			m_MaxHeight = FontCache::getFontCache()->getMaxCharacterHeight(m_fontKey.getTextInfo(), m_fontKey.getScale());
		}
		return m_MaxHeight;
	}
	std::shared_ptr<GlyphEntry> CharacterList::getCharacter(GuGuUtf8Str Char)
	{
		if (m_characters.find(Char) != m_characters.end())
			return m_characters[Char];

		std::shared_ptr<GlyphEntry> newEntry = std::make_shared<GlyphEntry>();
		m_characters.insert({ Char, newEntry });

		std::shared_ptr<FreeTypeFace> freeTypeFace = FontCache::getFontCache()->getFreeTypeFace(m_fontKey.getTextInfo());
		//GuGu_LOGD("%s:unicode:%d", Char.getStr(), Char.getUnicode().at(0));
		const uint32_t glyphIndex = FT_Get_Char_Index(freeTypeFace->getFontFace(), Char.getUnicode().at(0));

		//------get advance------
		int32_t xAdvance = 0;
		FT_Fixed advance;
		FT_Get_Advance(freeTypeFace->getFontFace(), glyphIndex, 0, &advance);
		xAdvance = ((advance + (1 << 9) >> 10) + (1 << 5)) >> 6;
		//------get advance------

		newEntry->Char = Char;
		newEntry->glyphIndex = glyphIndex;
		newEntry->xAdvance = xAdvance;
		FT_Load_Glyph(freeTypeFace->getFontFace(), glyphIndex, 0);
		FT_GlyphSlot slot = freeTypeFace->getFontFace()->glyph;
		newEntry->m_glyphFontAtlasData.verticalOffset = slot->bitmap_top;
		newEntry->m_glyphFontAtlasData.horizontalOffset = slot->bitmap_left;

		FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		FT_Bitmap* bitMap = nullptr;
		//FT_Bitmap tmpBitmap;
		bitMap = &slot->bitmap;
		const uint32_t bytesPerPixel = 1;
		std::vector<uint8_t> rawPixels(bitMap->rows * bitMap->width * bytesPerPixel);
		for (uint32_t row = 0; row < bitMap->rows; ++row)
		{
			std::memcpy(&rawPixels[row * bitMap->width * bytesPerPixel], &bitMap->buffer[row * bitMap->pitch], bitMap->width * bytesPerPixel);
		}
		const int32_t grayBoost = 255 / (bitMap->num_grays - 1);
		for (uint8_t& rawPixel : rawPixels)
		{
			rawPixel *= grayBoost;
		}

		uint16_t characterWidth = bitMap->width;
		uint16_t characterHeight = bitMap->rows;
		//GuGu_LOGD("font:%s size:%d, %d", Char.getStr(), characterWidth, characterHeight);
		//todo:add texture atlas
		std::shared_ptr<AtlasedTextureSlot> atlasedTextureSlot = FontCache::getFontCache()->addCharacter(characterWidth, characterHeight, rawPixels);
		newEntry->m_glyphFontAtlasData.startU = atlasedTextureSlot->x + atlasedTextureSlot->padding;
		newEntry->m_glyphFontAtlasData.startV = atlasedTextureSlot->y + atlasedTextureSlot->padding;
		newEntry->m_glyphFontAtlasData.vSize = atlasedTextureSlot->height - 2 * atlasedTextureSlot->padding;
		newEntry->m_glyphFontAtlasData.uSize = atlasedTextureSlot->width - 2 * atlasedTextureSlot->padding;

		FontCache::getFontCache()->setDirtyFlag(true);

		return newEntry;
	}
}