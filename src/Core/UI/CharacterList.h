#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

#include "TextInfo.h"

namespace GuGu {
	struct GlyphFontAtlasData
	{
		/*the vertical distance from the baseline to the topmost border of the glyph bitmap*/
		int16_t verticalOffset = 0;
		/*the horizontal distance from the origin to the leftmost border of the glyph bitmap*/
		int16_t horizontalOffset = 0;
		/*start x location of the glyph in the texture*/
		uint16_t startU = 0;
		/*start y location of the glyph in the texture*/
		uint16_t startV = 0;
		/*x size of the glyph in the texture*/
		uint16_t uSize = 0;
		/*y size of the glyph in the texture*/
		uint16_t vSize = 0;
		/*index to a specific texture*/
		uint8_t textureIndex = 0;
		/*true if this entry is valid, false otherwise*/
		bool valid = false;
	};
	struct GlyphEntry
	{
		GuGuUtf8Str Char;
		/*the index of this glyph in the FreeType face*/
		uint32_t glyphIndex = 0;
		/*the index of this glyph from the source text, the source indices may skip characters if the sequence contains ligatures, additionally, some characters produce multiple leading to duplicate souce indices*/
		uint32_t sourceIndex = 0;
		/*the amount to advance in x before drawing the next glyph in the sequence*/
		int16_t xAdvance = 0;
		/*the amount to advance in y before drawing the next glyph in the sequence*/
		int16_t yAdvance = 0;
		/*the offset to apply in x when drawing this glyph*/
		int16_t xOffset = 0;
		/*the offset to apply in y when drawing this glyph*/
		int16_t yOffset = 0;

		GlyphFontAtlasData m_glyphFontAtlasData;
	};
	//struct FontKey;
	class CharacterList
	{
	public:
		CharacterList(const FontKey& fontKey);

		virtual ~CharacterList();

		uint16_t getMaxHeight();

		std::shared_ptr<GlyphEntry> getCharacter(GuGuUtf8Str Char);
	private:
		FontKey m_fontKey;//key

		std::unordered_map<GuGuUtf8Str, std::shared_ptr<GlyphEntry>> m_characters;

		uint16_t m_MaxHeight;

		uint16_t m_BaseLine;
	};
}