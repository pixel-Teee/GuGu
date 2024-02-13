#pragma once

#include <unordered_map>

#include "TextInfo.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Renderer/nvrhi.h>
#include <Renderer/MyMath.h>
#include <list>

namespace GuGu {
	//class TextInfo;
	class CharacterList;
	class GuGuUtf8Str;
	class FreeTypeFace;
	struct AtlasedTextureSlot;
	struct FCopyRowData;
	class FontCache {
	public:
		FontCache();

		virtual ~FontCache();

		static std::shared_ptr<FontCache> getFontCache();

		math::double2 measureText(const GuGuUtf8Str& str, const TextInfo& textInfo);//todo:add font scale

		std::shared_ptr<CharacterList> getCharacterList(const TextInfo& textInfo);

		uint16_t getMaxCharacterHeight(const TextInfo& textInfo);

		std::shared_ptr<FreeTypeFace> getFreeTypeFace(const TextInfo& textInfo);

		std::shared_ptr<AtlasedTextureSlot> addCharacter(uint16_t inSizeX, uint16_t inSizeY, std::vector<uint8_t>& rawPixels);

		nvrhi::TextureHandle getFontAtlasTexture();

		std::vector<uint8_t>& getAtlasRawData();

		void setFontAtlasTexture(nvrhi::TextureHandle newTexture);

		bool getDirtyFlag();
		void setDirtyFlag(bool value);

		friend class CharacterList;
	private:
		void copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data);
		void copyRow(const FCopyRowData& copyRowData);
		void zeroRow(const FCopyRowData& copyRowData);
		std::unordered_map<TextInfo, std::shared_ptr<CharacterList>> m_characterLists;

		std::unordered_map<TextInfo, std::shared_ptr<FreeTypeFace>> m_faces;

		nvrhi::TextureHandle m_fontAtlasTexture;

		std::list<std::shared_ptr<AtlasedTextureSlot>> m_fontAtlasTextureEmptySlots;
		std::list<std::shared_ptr<AtlasedTextureSlot>> m_fontAtlasTextureUsedSlots;
		std::vector<uint8_t> m_atlasData;

		FT_Library m_library;

		uint32_t m_atlasSize;

		bool m_fontAtlasDirty;
	};
}