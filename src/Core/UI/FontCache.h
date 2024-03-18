#pragma once

#include <unordered_map>

#include "TextInfo.h"

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "CharacterList.h"//GlyphEntry depends on it

namespace GuGu {
	//class TextInfo;
	class GuGuUtf8Str;

	class CharacterList;
	class FreeTypeFace;

	struct FCopyRowData;
	class AtlasTexture;
	struct AtlasedTextureSlot;

	class FileSystem;

	enum class TextShapingMethod : uint8_t
	{
		Auto = 0,

		KerningOnly,

		FullShaping
	};

	//这个是针对渲染一个文本序列的信息
	class ShapedGlyphSequence
	{
	public:
		explicit ShapedGlyphSequence()
			: m_glyphsToRender()
			, m_textBaseLine(0)
			, m_maxTextHeight(0)
			, m_sequenceWidth(0)
			, m_sourceIndicesToGlyphData(SourceTextRange(0, 0))
		{}

		struct SourceTextRange
		{
			SourceTextRange(const int32_t inTextStart, const int32_t inTextLen)
				: textStart(inTextStart)
				, textLen(inTextLen)
			{}
			int32_t textStart;
			int32_t textLen;
		};

		struct SourceIndexToGlyphData
		{
			SourceIndexToGlyphData()
				: glyphIndex(-1)
				, additionalGlyphIndices()
			{}


			int32_t glyphIndex;
			std::vector<int32_t> additionalGlyphIndices;
		};

		struct SourceIndicesToGlyphData
		{
			explicit SourceIndicesToGlyphData(const SourceTextRange& inSourceTextRange)
				: m_sourceTextRange(inSourceTextRange)
				, m_glyphDataArray()
			{
				m_glyphDataArray.resize(inSourceTextRange.textLen);
			}

		private:
			SourceTextRange m_sourceTextRange;
			std::vector<SourceIndexToGlyphData> m_glyphDataArray;
		};

		std::vector<GlyphEntry> m_glyphsToRender;
		int16_t m_textBaseLine;
		uint16_t m_maxTextHeight;
		int32_t m_sequenceWidth;
		std::vector<std::weak_ptr<FreeTypeFace>> m_glyphFontFaces;
		/*一个从source indices 指向它们的 shaped glyph data 的索引*/
		SourceIndicesToGlyphData m_sourceIndicesToGlyphData;
	};
	
	class FontCache {
	public:
		FontCache();

		virtual ~FontCache();

		static std::shared_ptr<FontCache> getFontCache();

		void Init(std::shared_ptr<FileSystem> fileSystem);

		math::float2 measureText(const GuGuUtf8Str& str, const TextInfo& textInfo, float inScale);//todo:add font scale

		std::shared_ptr<AtlasedTextureSlot> addCharacter(uint16_t inSizeX, uint16_t inSizeY, std::vector<uint8_t>& rawPixels);

		std::shared_ptr<CharacterList> getCharacterList(const TextInfo& textInfo, float inScale);

		uint16_t getMaxCharacterHeight(const TextInfo& textInfo, float scale);

		int16_t getBaseLine(const TextInfo& textInfo, float scale);

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