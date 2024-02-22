#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GuGuUtf8Str;
	class GuGuFile;
	class FreeTypeFace {
	public:
		FreeTypeFace(FT_Library* library, GuGuUtf8Str& filePath);

        //FreeTypeFace();

		virtual ~FreeTypeFace();

		FT_Face getFontFace();
	private:
		struct FFTStreamHandler
		{
			FFTStreamHandler();
			FFTStreamHandler(const GuGuUtf8Str& inFileName);
			~FFTStreamHandler();
			static void CloseFile(FT_Stream inStream);
			static unsigned long ReadData(FT_Stream inStream, unsigned long inOffset, unsigned char* inBuffer, unsigned long inCount);
			std::shared_ptr<GuGuFile> m_fileHandler;
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