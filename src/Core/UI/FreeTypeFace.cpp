#include <pch.h>

#include "FreeTypeFace.h"

#include <Core/GuGuFile.h>
#include <Core/Archiver.h>

namespace GuGu {
	FreeTypeFace::FreeTypeFace(FT_Library* library, GuGuUtf8Str& filePath)
		: m_library(library)
		, m_filePath(filePath)
		, m_ftStreamHandler(filePath)
	{
		//load FT_Face
		std::memset(&m_ftStream, 0, sizeof(m_ftStream));
		m_ftStream.size = (uint32_t)m_ftStreamHandler.m_fileSize;
		m_ftStream.descriptor.pointer = &m_ftStreamHandler;
		m_ftStream.close = &FFTStreamHandler::CloseFile;
		m_ftStream.read = &FFTStreamHandler::ReadData;

		FT_Open_Args ftFaceOpenArgs;
		memset(&ftFaceOpenArgs, 0, sizeof(ftFaceOpenArgs));
		ftFaceOpenArgs.flags = FT_OPEN_STREAM;
		ftFaceOpenArgs.stream = &m_ftStream;

		FT_Face ftFace = nullptr;
		FT_Error error = FT_Open_Face(*m_library, &ftFaceOpenArgs, 0, &ftFace);
		
		m_ftFace = ftFace;

		//FT_Error error2 = FT_New_Face(*m_library, "STKAITI.TTF", 0, &ftFace);

		//m_ftFace = ftFace;
	}
	FreeTypeFace::~FreeTypeFace()
	{
		FT_Done_Face(m_ftFace);
	}
	FT_Face FreeTypeFace::getFontFace()
	{
		return m_ftFace;
	}

    //FreeTypeFace::FreeTypeFace() {
//
    //}

    FreeTypeFace::FFTStreamHandler::FFTStreamHandler()
	{
	}
	FreeTypeFace::FFTStreamHandler::FFTStreamHandler(const GuGuUtf8Str& inFileName)
	{
		m_fileHandler = CreateFileFactory();
#if 0
		m_fileHandler->OpenFile(inFileName, GuGuFile::FileMode::OnlyRead);
		m_fileSize = m_fileHandler->getFileSize();
#else 
		m_fileHandler->OpenFile("archiver.bin", GuGuFile::FileMode::OnlyRead);
		m_fileSize = getFileSize(inFileName);
		m_filePath = inFileName;
#endif	
	}
	FreeTypeFace::FFTStreamHandler::~FFTStreamHandler()
	{
	}
	void FreeTypeFace::FFTStreamHandler::CloseFile(FT_Stream inStream)
	{
		FFTStreamHandler* myStreamHandler = (FFTStreamHandler*)inStream->descriptor.pointer;
		if (myStreamHandler->m_fileHandler) //todo:may be need to clean
		{
			myStreamHandler->m_fileHandler->CloseFile();
		}
	}
	unsigned long FreeTypeFace::FFTStreamHandler::ReadData(FT_Stream inStream, unsigned long inOffset, unsigned char* inBuffer, unsigned long inCount)
	{
		FFTStreamHandler* myStreamHandler = (FFTStreamHandler*)inStream->descriptor.pointer;
		if (myStreamHandler->m_fileHandler)
		{
#if 0
			if (!myStreamHandler->m_fileHandler->Seek(inOffset))
			{
				return 0;
			}
#else
			if (!GuGu::Seek(myStreamHandler->m_fileHandler, myStreamHandler->m_filePath, inOffset, GuGuFile::SeekDir::Begin))
			{
				GuGu_LOGD("%d\n", inOffset);
				return 0;
			}
#endif
		}

		if (inCount > 0)
		{
			if (myStreamHandler->m_fileHandler)
			{
#if 0
				int32_t bytesHaveReaded = 0;
				if (!myStreamHandler->m_fileHandler->ReadFile(inBuffer, inCount, bytesHaveReaded))
				{
					return 0;
				}
#else
				int32_t bytesHaveReaded = 0;
				if (!ReadArchive(myStreamHandler->m_fileHandler, myStreamHandler->m_filePath, inBuffer, inCount))
				{
					return 0;
				}
#endif
			}
			else
			{
				return 0;
			}
		}

		return inCount;
	}
}