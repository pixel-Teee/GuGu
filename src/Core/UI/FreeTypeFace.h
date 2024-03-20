#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Core/GuGuUtf8Str.h>

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
}