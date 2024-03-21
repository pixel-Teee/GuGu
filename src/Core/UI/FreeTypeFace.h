#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Core/GuGuUtf8Str.h>
#include <Core/HashCombine.h>

#include <unordered_map>

namespace GuGu {
	class GuGuFile;
	class GuGuUtf8Str;
	class FileSystem;
	class FreeTypeFace {
	public:
		FreeTypeFace(FT_Library* library, GuGuUtf8Str& filePath, std::shared_ptr<FileSystem> fileSystem);

        //FreeTypeFace();

		virtual ~FreeTypeFace();

		FT_Face getFontFace();

		FT_Pos getDescender() const;

		FT_Pos getScaledHeight() const;
	private:
		struct FFTStreamHandler
		{
			FFTStreamHandler();
			FFTStreamHandler(const GuGuUtf8Str& inFileName, std::shared_ptr<FileSystem> fileSystem);
			~FFTStreamHandler();
			static void CloseFile(FT_Stream inStream);
			static unsigned long ReadData(FT_Stream inStream, unsigned long inOffset, unsigned char* inBuffer, unsigned long inCount);
			std::shared_ptr<FileSystem> m_fileSystem;
			uint64_t m_fileSize;
			GuGuUtf8Str m_filePath;
		};

		GuGuUtf8Str m_filePath;
		FT_Face m_ftFace;
		FT_Library* m_library;
		FT_StreamRec m_ftStream;
		FFTStreamHandler m_ftStreamHandler;
	};

	class FreeTypeAdvanceCache
	{
	public:
		FreeTypeAdvanceCache(FT_Face inFace, const int32_t inLoadFlags, const int32_t inFontSize, const float inFontScale);

		bool findOrCache(const uint32_t inGlyphIndex, FT_Fixed& outCachedAdvance);
	private:

		FT_Face face;
		const int32_t loadFlags;
		const int32_t fontSize;
		const float fontScale;
		std::unordered_map<uint32_t, FT_Fixed> advanceMap;
	};

	class FreeTypeCacheFontKey
	{
	public:
		FreeTypeCacheFontKey(FT_Face inFace, const int32_t inFlags, const int32_t inFontSize, const float inFontScale)
			: face(inFace)
			, flags(inFlags)
			, fontSize(inFontSize)
			, fontScale(inFontScale)
		{}

		bool operator==(const FreeTypeCacheFontKey& rhs) const
		{
			return face == rhs.face && flags == rhs.flags && fontSize == rhs.fontSize && fontScale == rhs.fontScale;
		}
	
		FT_Face face;
		int32_t flags;
		int32_t fontSize;
		float fontScale;
	};
}

namespace std
{
	template<>
	struct hash<GuGu::FreeTypeCacheFontKey>
	{
		size_t operator()(const GuGu::FreeTypeCacheFontKey& s) const
		{
			size_t keyHash = 0;
			GuGu::hash_combine(keyHash, s.face);
			GuGu::hash_combine(keyHash, s.flags);
			GuGu::hash_combine(keyHash, s.fontSize);
			GuGu::hash_combine(keyHash, s.fontScale);
			return keyHash;
		}
	};
}

namespace GuGu
{

	class FreeTypeCacheDirectory
	{
	public:
		std::shared_ptr<FreeTypeAdvanceCache> getAdvanceCache(FT_Face inFace, const int32_t inLoadFlags, const int32_t inFontSize, const float inFontScale);

		std::unordered_map<FreeTypeCacheFontKey, std::shared_ptr< FreeTypeAdvanceCache>> advanceCacheMap;
	};

}
