#pragma once

#include <functional>
#include <unordered_map>

#include "TextInfo.h"

#include <Core/Math/MyMath.h>
#include <Renderer/nvrhi.h>

#include <list>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "CharacterList.h"//GlyphEntry depends on it

#include <Core/HashCombine.h>

namespace GuGu {
	//class TextInfo;
	class GuGuUtf8Str;

	class CharacterList;
	class FreeTypeFace;
	class FontCache;

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

		ShapedGlyphSequence(std::vector<GlyphEntry> inGlyphsToRender, const int16_t inTextBaseLine, const uint16_t inMaxTextHeight, const SourceTextRange& inSourceTextRange);

		struct GlyphOffsetResult
		{
			GlyphOffsetResult()
				: m_glyph(nullptr)
				, m_glyphOffset(0)
				, m_characterIndex(0)
			{}

			GlyphOffsetResult(const int32_t inCharacterIndex)
				: m_glyph(nullptr)
				, m_glyphOffset(0)
				, m_characterIndex(inCharacterIndex)
			{}

			GlyphOffsetResult(const GlyphEntry* inGlyph, const int32_t inGlyphOffset)
				: m_glyph(inGlyph)
				, m_glyphOffset(inGlyphOffset)
				, m_characterIndex(inGlyph->sourceIndex)
			{}
			//被碰撞到的的glyph
			const GlyphEntry* m_glyph;
			//碰撞到的glyph的左边的偏移
			int32_t m_glyphOffset;
			//被碰撞到的glyph index
			int32_t m_characterIndex;
		};

		struct SourceIndexToGlyphData
		{
			SourceIndexToGlyphData()
				: glyphIndex(-1)
				, additionalGlyphIndices()
			{}

			SourceIndexToGlyphData(const int32_t inGlyphIndex)
				: glyphIndex(inGlyphIndex)
				, additionalGlyphIndices()
			{}

			int32_t getLowestGlyphIndex() const
			{
				return glyphIndex;
			}

			int32_t getHighestGlyphIndex() const
			{
				return (additionalGlyphIndices.size() > 0) ? additionalGlyphIndices.back() : glyphIndex;
			}

			bool isValid() const
			{
				return glyphIndex != -1;
			}

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

			SourceIndexToGlyphData* getGlyphData(const int32_t inSourceTextIndex)
			{
				const int32_t internalIndex = inSourceTextIndex - m_sourceTextRange.textStart;
				return (internalIndex >= 0 && internalIndex < m_glyphDataArray.size()) ? &m_glyphDataArray[internalIndex] : nullptr;
			}

			const SourceIndexToGlyphData* getGlyphData(const int32_t inSourceTextIndex) const
			{
				const int32_t internalIndex = inSourceTextIndex - m_sourceTextRange.textStart;
				return (internalIndex >= 0 && internalIndex < m_glyphDataArray.size()) ? &m_glyphDataArray[internalIndex] : nullptr;
			}

			int32_t getSourceTextStartIndex() const
			{
				return m_sourceTextRange.textStart;
			}

			int32_t getSourceTextEndIndex() const
			{
				return m_sourceTextRange.textStart + m_sourceTextRange.textLen;
			}

		private:
			SourceTextRange m_sourceTextRange;
			std::vector<SourceIndexToGlyphData> m_glyphDataArray;
		};

		uint16_t getMaxTextHeight() const
		{
			return m_maxTextHeight;
		}

		std::optional<int32_t> getMeasuredWidth(const int32_t inStartIndex, const int32_t inEndIndex) const;

		typedef std::function<bool(const GlyphEntry&, int32_t)> eachShapedGlyphEntryCallback;
		void EnumerateLogicalGlyphsInSourceRange(const int32_t InStartIndex, const int32_t InEndIndex, const eachShapedGlyphEntryCallback& InGlyphCallback) const;
		void EnumerateVisualGlyphsInSourceRange(const int32_t InStartIndex, const int32_t InEndIndex, const eachShapedGlyphEntryCallback& InGlyphCallback) const;

		std::optional<GlyphOffsetResult> getGlyphAtOffset(FontCache& inFontCache, const int32_t inStartIndex, const int32_t inEndIndex, const int32_t inHorizontalOffset, const int32_t inStartOffset = 0) const;

		bool hasFoundGlyphAtOffset(FontCache& inFontCache, const int32_t inHorizontalOffset, const GlyphEntry& inCurrentEntry, const int32_t inCurrentGlyphIndex, int32_t& inOutCurrentOffset,
			const GlyphEntry*& outMatchedGlyph) const;

		std::vector<GlyphEntry> m_glyphsToRender;
		int16_t m_textBaseLine;
		uint16_t m_maxTextHeight;
		int32_t m_sequenceWidth;
		std::vector<std::weak_ptr<FreeTypeFace>> m_glyphFontFaces;
		/*一个从source indices 指向它们的 shaped glyph data 的索引*/
		SourceIndicesToGlyphData m_sourceIndicesToGlyphData;
	};

	struct KerningOnlyTextSequenceEntry
	{
		int32_t textStartIndex;
		int32_t textLength;
		GuGuUtf8Str filePath;
		std::shared_ptr<FreeTypeFace> faceAndMemory;

		KerningOnlyTextSequenceEntry(const int32_t inTextStartIndex, const int32_t inTextLen, const GuGuUtf8Str inFilePath, std::shared_ptr<FreeTypeFace> inFaceAndMemory)
			: textStartIndex(inTextStartIndex)
			, textLength(inTextLen)
			, filePath(inFilePath)
			, faceAndMemory(inFaceAndMemory)
		{}
	};

	struct ShapedGlyphEntryKey
	{
	public:
		ShapedGlyphEntryKey(std::shared_ptr<FreeTypeFace> inFreeTypeFace, int32_t fontSize, float fontScale, uint32_t glyphIndex);

		bool operator==(const ShapedGlyphEntryKey& rhs) const
		{
			return m_fontFace.lock() == rhs.m_fontFace.lock() && m_fontSize == rhs.m_fontSize && m_fontScale == rhs.m_fontScale && m_glyphIndex == rhs.m_glyphIndex;
		}
	
		std::weak_ptr<FreeTypeFace> m_fontFace;

		int32_t m_fontSize;

		float m_fontScale;

		uint32_t m_glyphIndex;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::ShapedGlyphEntryKey>
	{
		size_t operator()(const GuGu::ShapedGlyphEntryKey& s) const
		{
			size_t keyHash = 0;
			GuGu::hash_combine(keyHash, s.m_fontFace.lock().get());
			GuGu::hash_combine(keyHash, s.m_fontSize);
			GuGu::hash_combine(keyHash, s.m_fontScale);
			GuGu::hash_combine(keyHash, s.m_glyphIndex);
			return keyHash;
		}
	};
}



namespace GuGu{
	class FreeTypeCacheDirectory;
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

		std::shared_ptr<ShapedGlyphSequence> shapeUnidirectionalText(const GuGuUtf8Str& inText, const int32_t InTextStart, const int32_t InTextLen, const TextInfo& inFontInfo, const float inFontScale, const TextShapingMethod inTextShapingMethod) const;

		GlyphFontAtlasData getShapedGlyphFontAtlasData(const GlyphEntry& inShapedGlyph);

		friend class CharacterList;
	private:
		void performTextShaping(const GuGuUtf8Str& inText, const int32_t inTextStart, const int32_t inTextLen, const TextInfo& inFontInfo, const float inFontScale, const TextShapingMethod textShapingMethod, std::vector<GlyphEntry>& outGlyphsToRender) const;

		void performKerningOnlyTextShaping(const GuGuUtf8Str& inText, const int32_t inTextStart, const int32_t inTextLen, const TextInfo& inFontInfo, const float inFontScale, std::vector<GlyphEntry>& outGlyphsToRender) const;

		std::shared_ptr<ShapedGlyphSequence> finalizeTextShaping(std::vector<GlyphEntry> inGlyphsToRender, const TextInfo& inFontInfo, const float inFontScale, const ShapedGlyphSequence::SourceTextRange& inSourceTextRange) const;
		//void copyDataIntoSlot(std::shared_ptr<AtlasedTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data);
		//void copyRow(const FCopyRowData& copyRowData);
		//void zeroRow(const FCopyRowData& copyRowData);
		std::unordered_map<FontKey, std::shared_ptr<CharacterList>> m_characterLists;

		std::unordered_map<TextInfo, std::shared_ptr<FreeTypeFace>> m_faces;

		std::unordered_map<ShapedGlyphEntryKey, std::shared_ptr<GlyphFontAtlasData>> m_shapedGlyphToAtlasData;

		std::shared_ptr<FreeTypeCacheDirectory> m_freeTypeCacheDirectory;

		std::shared_ptr<AtlasTexture> m_atlasTexture;

		FT_Library m_library;

		uint32_t m_atlasSize;

		bool m_fontAtlasDirty;

		std::shared_ptr<FileSystem> m_fileSystem;

		
	};
}