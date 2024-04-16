#include <pch.h>

#include "FontCache.h"

#include "TextInfo.h"
#include "CharacterList.h"
#include "FreeTypeFace.h"
#include "FreeTypeUtils.h"

#include "AtlasTexture.h"

#include <Core/Math/MyMath.h>

#include <freetype/ftadvanc.h>

namespace GuGu {
	ShapedGlyphSequence::ShapedGlyphSequence(std::vector<GlyphEntry> inGlyphsToRender, const int16_t inTextBaseLine, const uint16_t inMaxTextHeight, const SourceTextRange& inSourceTextRange)
		: m_glyphsToRender(inGlyphsToRender)
		, m_textBaseLine(inTextBaseLine)
		, m_maxTextHeight(inMaxTextHeight)
		, m_sequenceWidth(0)
		, m_sourceIndicesToGlyphData(inSourceTextRange)
	{
		const int32_t numGlyphsToRender = m_glyphsToRender.size();

		for (int32_t currentGlyphIndex = 0; currentGlyphIndex < numGlyphsToRender; ++currentGlyphIndex)
		{
			const GlyphEntry& currentGlyph = m_glyphsToRender[currentGlyphIndex];

			m_sequenceWidth += currentGlyph.xAdvance;

			SourceIndexToGlyphData* sourceIndexToGlyphData = m_sourceIndicesToGlyphData.getGlyphData(currentGlyph.sourceIndex);

			if (sourceIndexToGlyphData->isValid())
			{
				sourceIndexToGlyphData->additionalGlyphIndices.push_back(currentGlyphIndex);
			}
			else
			{
				*sourceIndexToGlyphData = SourceIndexToGlyphData(currentGlyphIndex);
			}
		}
	}
	std::optional<int32_t> ShapedGlyphSequence::getMeasuredWidth(const int32_t inStartIndex, const int32_t inEndIndex) const
	{
		int32_t measuredWidth = 0;

		auto glyphCallback = [&](const GlyphEntry& currentGlyph, int32_t currentGlyphIndex) -> bool
		{
			measuredWidth += currentGlyph.xAdvance;
			return true;
		};

		EnumerateLogicalGlyphsInSourceRange(inStartIndex, inEndIndex, glyphCallback);

		return measuredWidth;
	}
	void ShapedGlyphSequence::EnumerateLogicalGlyphsInSourceRange(const int32_t InStartIndex, const int32_t InEndIndex, const eachShapedGlyphEntryCallback& InGlyphCallback) const
	{
		if (InStartIndex == InEndIndex)
			return;

		int32_t sourceIndex = InStartIndex;
		while (sourceIndex < InEndIndex)
		{
			//获取与这个source index 相符合的 glyphs
			const SourceIndexToGlyphData* sourceIndexToGlyphData = m_sourceIndicesToGlyphData.getGlyphData(sourceIndex);

			if (!sourceIndexToGlyphData)
				return;

			//枚举每个被相应 source index 生成的glyph
			const int32_t startGlyphIndex = sourceIndexToGlyphData->getLowestGlyphIndex();
			const int32_t endGlyphIndex = sourceIndexToGlyphData->getHighestGlyphIndex();

			for (int32_t currentGlyphIndex = startGlyphIndex; currentGlyphIndex <= endGlyphIndex; ++currentGlyphIndex)
			{
				const GlyphEntry& currentGlyph = m_glyphsToRender[currentGlyphIndex];

				InGlyphCallback(currentGlyph, currentGlyphIndex);

				sourceIndex += 1;//todo：这里之后需要修改为NumCharactersInGlyph，多个字母可能表示一个字符
			}
		}
	}
	void ShapedGlyphSequence::EnumerateVisualGlyphsInSourceRange(const int32_t InStartIndex, const int32_t InEndIndex, const eachShapedGlyphEntryCallback& InGlyphCallback) const
	{
		if (InStartIndex == InEndIndex)
			return;

		const SourceIndexToGlyphData* startSourceIndexToGlyphData = m_sourceIndicesToGlyphData.getGlyphData(InStartIndex);
		const SourceIndexToGlyphData* endSourceIndexToGlyphData = m_sourceIndicesToGlyphData.getGlyphData(InEndIndex - 1);

		int32_t startGlyphIndex = -1;
		int32_t endGlyphIndex = -1;
		if (startSourceIndexToGlyphData->glyphIndex <= endSourceIndexToGlyphData->glyphIndex)
		{
			startGlyphIndex = startSourceIndexToGlyphData->getLowestGlyphIndex();
			endGlyphIndex = endSourceIndexToGlyphData->getHighestGlyphIndex();
		}
		else
		{
			startGlyphIndex = endSourceIndexToGlyphData->getLowestGlyphIndex();
			endGlyphIndex = startSourceIndexToGlyphData->getHighestGlyphIndex();
		}

		bool bStartIndexInRange = m_sourceIndicesToGlyphData.getSourceTextStartIndex() == InStartIndex;
		bool bEndIndexInRange = m_sourceIndicesToGlyphData.getSourceTextEndIndex() == InEndIndex;

		for (int32_t currentGlyphIndex = startGlyphIndex; currentGlyphIndex <= endGlyphIndex; ++currentGlyphIndex)
		{
			const GlyphEntry& currentGlyph = m_glyphsToRender[currentGlyphIndex];

			if (!bStartIndexInRange || !bEndIndexInRange)
			{
				const int32_t glyphStartSourceIndex = currentGlyph.sourceIndex;
				const int32_t glyphEndSourceIndex = currentGlyph.sourceIndex + 1;//todo:这里要修复，可能一个字母占用多个glyph

				if (!bStartIndexInRange && glyphStartSourceIndex == InStartIndex)
					bStartIndexInRange = true;

				if (!bEndIndexInRange && glyphEndSourceIndex == InEndIndex)
					bEndIndexInRange = true;
			}

			if (!InGlyphCallback(currentGlyph, currentGlyphIndex))
				return;
		}

		return;
	}
	std::optional<ShapedGlyphSequence::GlyphOffsetResult> ShapedGlyphSequence::getGlyphAtOffset(FontCache& inFontCache, const int32_t inStartIndex, const int32_t inEndIndex, const int32_t inHorizontalOffset, const int32_t inStartOffset) const
	{
		int32_t currentOffset = inStartOffset;

		const GlyphEntry* matchedGlyph = nullptr;
		const GlyphEntry* rightMostGlyph = nullptr;

		auto glyphCallbck = [&](const GlyphEntry& currentGlyph, int32_t currentGlyphIndex)->bool
		{
			if (hasFoundGlyphAtOffset(inFontCache, inHorizontalOffset, currentGlyph, currentGlyphIndex, /*out*/currentOffset, /*out*/matchedGlyph))
				return false;
			rightMostGlyph = &currentGlyph;
			return true;
		};

		EnumerateVisualGlyphsInSourceRange(inStartIndex, inEndIndex, glyphCallbck);

		if (matchedGlyph)
		{
			return GlyphOffsetResult(matchedGlyph, currentOffset);
		}

		//todo:如果点击的位置超出边界，那么就根据文本的方向，选择第一个或者最后一个位置
		//if (!rightMostGlyph)
		//{
		if (inEndIndex >= m_sourceIndicesToGlyphData.getSourceTextStartIndex() && inEndIndex <= m_sourceIndicesToGlyphData.getSourceTextEndIndex())
			return GlyphOffsetResult(inEndIndex);
		//}
		//else
		//{
		//	if (inStartIndex >= m_sourceIndicesToGlyphData.getSourceTextStartIndex() && inEndIndex <= m_sourceIndicesToGlyphData.getSourceTextEndIndex())
		//		return GlyphOffsetResult(inStartIndex);
		//}

		return std::optional<GlyphOffsetResult>();
	}
	bool ShapedGlyphSequence::hasFoundGlyphAtOffset(FontCache& inFontCache, const int32_t inHorizontalOffset, const GlyphEntry& inCurrentEntry, const int32_t inCurrentGlyphIndex, int32_t& inOutCurrentOffset, const GlyphEntry*& outMatchedGlyph) const
	{
		
		int32_t totalGlyphSpacing = 0;
		int32_t totalGlyphAdvance = 0;
		for (int32_t subGlyphIndex = inCurrentGlyphIndex; ;++subGlyphIndex)
		{
			const GlyphEntry& subGlyph = m_glyphsToRender[subGlyphIndex];
			const GlyphFontAtlasData& subGlyphAtlasData = inFontCache.getShapedGlyphFontAtlasData(subGlyph);
			totalGlyphSpacing += subGlyphAtlasData.horizontalOffset + subGlyph.xAdvance;
			totalGlyphAdvance += subGlyph.xAdvance;

			const bool bIsWithinGlyphCluster = (subGlyphIndex + 1) >= 0 && (subGlyphIndex + 1) < m_glyphsToRender.size() && subGlyph.sourceIndex == m_glyphsToRender[subGlyphIndex + 1].sourceIndex;
			if(!bIsWithinGlyphCluster)
				break;
		}

		const int32_t glyphWidthToTest = totalGlyphSpacing / 2.0f;

		if (inHorizontalOffset < (inOutCurrentOffset + glyphWidthToTest))
		{
			outMatchedGlyph = &inCurrentEntry;

			return true;
		}

		inOutCurrentOffset += totalGlyphAdvance;
		return false;
	}
	FontCache::FontCache()
		: m_freeTypeCacheDirectory(std::make_shared<FreeTypeCacheDirectory>())
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

				const int32_t totalCharSpacing = entry->m_glyphFontAtlasData->horizontalOffset + entry->xAdvance;

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
	int16_t FontCache::getBaseLine(const TextInfo& textInfo, float scale)
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

		FT_Pos baseLine = FT_MulFix(freeTypeFace->getFontFace()->size->metrics.descender, freeTypeFace->getFontFace()->size->metrics.y_scale);

		return ((baseLine + (1 << 5)) >> 6);
	}
	std::shared_ptr<FreeTypeFace> FontCache::getFreeTypeFace(const TextInfo& textInfo)
	{
		std::shared_ptr<FreeTypeFace> freeTypeFace;
		if (m_faces.find(textInfo) != m_faces.end())
			freeTypeFace = m_faces[textInfo];
		else
		{
			GuGuUtf8Str filePath = textInfo.getFilePath();
			freeTypeFace = std::make_shared<FreeTypeFace>(&m_library, filePath, m_fileSystem);
			m_faces.insert({ textInfo, freeTypeFace });
		}
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
	std::shared_ptr<ShapedGlyphSequence> FontCache::shapeUnidirectionalText(const GuGuUtf8Str& inText, const int32_t InTextStart, const int32_t InTextLen, const TextInfo& inFontInfo, const float inFontScale, const TextShapingMethod inTextShapingMethod) const
	{
		//todo：暂时不用 harfbuzz 来塑形

		std::vector<GlyphEntry> glyphsToRender;
		performTextShaping(inText, InTextStart, InTextLen, inFontInfo, inFontScale, inTextShapingMethod, glyphsToRender);
		return finalizeTextShaping(std::move(glyphsToRender), inFontInfo, inFontScale, ShapedGlyphSequence::SourceTextRange(InTextStart, InTextLen));
	}
	GlyphFontAtlasData FontCache::getShapedGlyphFontAtlasData(const GlyphEntry& inShapedGlyph)
	{
		std::shared_ptr<GlyphFontAtlasData> cachedAtlasData = inShapedGlyph.m_glyphFontAtlasData;

		if (cachedAtlasData != nullptr)
		{
			return *cachedAtlasData;
		}

		ShapedGlyphEntryKey glyphKey(inShapedGlyph.m_fontFace.lock(), inShapedGlyph.fontSize, inShapedGlyph.fontScale, inShapedGlyph.glyphIndex);

		auto it = m_shapedGlyphToAtlasData.find(glyphKey);

		if (it != m_shapedGlyphToAtlasData.end())
		{
			inShapedGlyph.m_glyphFontAtlasData = (*it).second;
			return *inShapedGlyph.m_glyphFontAtlasData;
		}

		std::shared_ptr<GlyphFontAtlasData> newAtlasData = std::make_shared<GlyphFontAtlasData>();
		FT_Load_Glyph(inShapedGlyph.m_fontFace.lock()->getFontFace(), inShapedGlyph.glyphIndex, 0);
		FT_GlyphSlot slot = inShapedGlyph.m_fontFace.lock()->getFontFace()->glyph;
		newAtlasData->verticalOffset = slot->bitmap_top;
		newAtlasData->horizontalOffset = slot->bitmap_left;
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
		newAtlasData->startU = atlasedTextureSlot->x + atlasedTextureSlot->padding;
		newAtlasData->startV = atlasedTextureSlot->y + atlasedTextureSlot->padding;
		newAtlasData->vSize = atlasedTextureSlot->height - 2 * atlasedTextureSlot->padding;
		newAtlasData->uSize = atlasedTextureSlot->width - 2 * atlasedTextureSlot->padding;

		FontCache::getFontCache()->setDirtyFlag(true);
		m_shapedGlyphToAtlasData.insert({ glyphKey, newAtlasData });
		inShapedGlyph.m_glyphFontAtlasData = newAtlasData;
		return *newAtlasData;
	}
	void FontCache::performTextShaping(const GuGuUtf8Str& inText, const int32_t inTextStart, const int32_t inTextLen, const TextInfo& inFontInfo, const float inFontScale, const TextShapingMethod textShapingMethod, std::vector<GlyphEntry>& outGlyphsToRender) const
	{
		if (inTextLen > 0)
		{
			performKerningOnlyTextShaping(inText, inTextStart, inTextLen, inFontInfo, inFontScale, outGlyphsToRender);
		}
	}
	void FontCache::performKerningOnlyTextShaping(const GuGuUtf8Str& inText, const int32_t inTextStart, const int32_t inTextLen, const TextInfo& inFontInfo, const float inFontScale, std::vector<GlyphEntry>& outGlyphsToRender) const
	{
		std::vector<KerningOnlyTextSequenceEntry> kerningOnlyTextSequence;

		//步骤1：分割文本到段，对于相同的font face 进行分割
		{
			//暂时只用一种文本对于TextInfo
			int32_t splitStartIndex = inTextStart;
			int32_t runningTextIndex = inTextStart;
			GuGuUtf8Str fontFilePath("");
			std::shared_ptr<FreeTypeFace> runningFaceAndMemory;
			float runningSubFontScalingFactor = 1.0f;

			auto appendPendingFontDataToSequence = [&]()
			{
				if (fontFilePath != u8"")
				{
					kerningOnlyTextSequence.emplace_back(
						splitStartIndex,					
						runningTextIndex - splitStartIndex,
						fontFilePath,
						runningFaceAndMemory
					);

					fontFilePath = "";
					runningFaceAndMemory.reset();
					runningSubFontScalingFactor = 1.0f;
				}
			};

			const int32_t textEndIndex = inTextStart + inTextLen;
			for (; runningTextIndex < textEndIndex; ++runningTextIndex)
			{
				const GuGuUtf8Str currentChar = inText[runningTextIndex];

				const bool bShouldRenderAsWhitespace = false;//todo:修复这里

				GuGuUtf8Str filePath = inFontInfo.m_textPath;
				std::shared_ptr<FreeTypeFace> face = FontCache::getFontCache()->getFreeTypeFace(inFontInfo);

				if (fontFilePath != "" || fontFilePath != filePath || face != runningFaceAndMemory)
				{
					appendPendingFontDataToSequence();

					fontFilePath = filePath;
					splitStartIndex = runningTextIndex;
					runningFaceAndMemory = face;
				}
			}

			appendPendingFontDataToSequence();
		}

		//步骤2：目前我们使用font cache 去获取每个字母的大小，还有每个字母对的kerning
		{
			outGlyphsToRender.reserve(outGlyphsToRender.size() + inTextLen);

			for (const KerningOnlyTextSequenceEntry& kerningOnlyTextSequenceEntry : kerningOnlyTextSequence)
			{
				if (!kerningOnlyTextSequenceEntry.faceAndMemory)
				{
					continue;
				}

				const bool bHasKerning = FT_HAS_KERNING(kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace());

				uint32_t glyphFlags = 0;

				FreeTypeUtils::ApplySizeAndScale(kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace(), inFontInfo.m_size, inFontScale);
				std::shared_ptr<FreeTypeAdvanceCache> advanceCache = m_freeTypeCacheDirectory->getAdvanceCache(kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace(), 0, inFontInfo.m_size, inFontScale);

				for (int32_t sequenceCharIndex = 0; sequenceCharIndex < kerningOnlyTextSequenceEntry.textLength; ++sequenceCharIndex)
				{
					const int32_t currentCharIndex = kerningOnlyTextSequenceEntry.textStartIndex + sequenceCharIndex;
					const GuGuUtf8Str currentChar = inText[currentCharIndex];

					//todo:这里要处理不能显示的字符
					uint32_t glyphIndex = FT_Get_Char_Index(kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace(), currentChar.getUnicode().at(0));

					int16_t xAdvance = 0;
					{
						FT_Fixed cachedAdvanceData = 0;
						if (advanceCache->findOrCache(glyphIndex, cachedAdvanceData))
						{
							xAdvance = ((((cachedAdvanceData + (1 << 9)) >> 10) + (1 << 5)) >> 6);
						}
						//FT_Error error = FT_Get_Advance(kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace(), glyphIndex, 0, &cachedAdvanceData);
						//if (error == 0)
						//{
						//	cachedAdvanceData = FT_MulFix(cachedAdvanceData, kerningOnlyTextSequenceEntry.faceAndMemory->getFontFace()->size->metrics.x_scale);
						//	xAdvance = ((((cachedAdvanceData + (1 << 9)) >> 10) + (1 << 5)) >> 6);
						//}
					}

					outGlyphsToRender.push_back(GlyphEntry());
					const int32_t currentGlyphEntryIndex = outGlyphsToRender.size() - 1;
					GlyphEntry& shapedGlyphEntry = outGlyphsToRender[currentGlyphEntryIndex];
					shapedGlyphEntry.fontSize = inFontInfo.m_size;
					shapedGlyphEntry.fontScale = inFontScale;
					shapedGlyphEntry.m_fontFace = kerningOnlyTextSequenceEntry.faceAndMemory;
					shapedGlyphEntry.glyphIndex = glyphIndex;
					shapedGlyphEntry.sourceIndex = currentCharIndex;
					shapedGlyphEntry.xAdvance = xAdvance;
					shapedGlyphEntry.yAdvance = 0;
					shapedGlyphEntry.xOffset = 0;
					shapedGlyphEntry.yOffset = 0;
					shapedGlyphEntry.Char = currentChar;
					
					//暂时不考虑kerning
				}
			}
		}
	}
	std::shared_ptr<ShapedGlyphSequence> FontCache::finalizeTextShaping(std::vector<GlyphEntry> inGlyphsToRender, const TextInfo& inFontInfo, const float inFontScale, const ShapedGlyphSequence::SourceTextRange& inSourceTextRange) const
	{
		int16_t textBaseLine = 0;
		uint16_t maxHeight = 0;
		std::shared_ptr<FreeTypeFace> face = FontCache::getFontCache()->getFreeTypeFace(inFontInfo);;
		FreeTypeUtils::ApplySizeAndScale(face->getFontFace(), inFontInfo.m_size, inFontScale);
		//获取base line和max height大小
		textBaseLine = ((face->getDescender() + (1 << 5)) >> 6);
		maxHeight = ((face->getScaledHeight() + (1 << 5)) >> 6);
		return std::make_shared<ShapedGlyphSequence>(inGlyphsToRender, textBaseLine, maxHeight, inSourceTextRange);
	}

	ShapedGlyphEntryKey::ShapedGlyphEntryKey(std::shared_ptr<FreeTypeFace> inFreeTypeFace, int32_t fontSize, float fontScale, uint32_t glyphIndex)
		: m_fontFace(inFreeTypeFace)
		, m_fontSize(fontSize)
		, m_fontScale(fontScale)
		, m_glyphIndex(glyphIndex)
	{
	}



}