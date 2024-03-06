#pragma once

#include <unordered_map>

#include "TextInfo.h"

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace GuGu {
	//class TextInfo;
	class GuGuUtf8Str;

	class CharacterList;
	class FreeTypeFace;

	struct FCopyRowData;
	class AtlasTexture;
	struct AtlasedTextureSlot;

	class FileSystem;
	
	class FontCache {
	public:
		FontCache();

		virtual ~FontCache();

		static std::shared_ptr<FontCache> getFontCache();

		void Init(std::shared_ptr<FileSystem> fileSystem);

		math::double2 measureText(const GuGuUtf8Str& str, const TextInfo& textInfo, float inScale);//todo:add font scale

		std::shared_ptr<AtlasedTextureSlot> addCharacter(uint16_t inSizeX, uint16_t inSizeY, std::vector<uint8_t>& rawPixels);

		std::shared_ptr<CharacterList> getCharacterList(const TextInfo& textInfo, float inScale);

		uint16_t getMaxCharacterHeight(const TextInfo& textInfo, float scale);

		std::shared_ptr<FreeTypeFace> getFreeTypeFace(const TextInfo& textInfo);		

		void setFontAtlasTexture(nvrhi::TextureHandle newTexture);

		nvrhi::TextureHandle getFontAtlasTexture();

		std::vector<uint8_t>& getAtlasRawData();

		bool getDirtyFlag();

		void setDirtyFlag(bool value);

		void clear();

		friend class CharacterList;
	private:
		//void copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data);
		//void copyRow(const FCopyRowData& copyRowData);
		//void zeroRow(const FCopyRowData& copyRowData);
		std::unordered_map<FontKey, std::shared_ptr<CharacterList>> m_characterLists;

		std::unordered_map<TextInfo, std::shared_ptr<FreeTypeFace>> m_faces;

		std::shared_ptr<AtlasTexture> m_atlasTexture;

		FT_Library m_library;

		uint32_t m_atlasSize;

		bool m_fontAtlasDirty;

		std::shared_ptr<FileSystem> m_fileSystem;
	};
}