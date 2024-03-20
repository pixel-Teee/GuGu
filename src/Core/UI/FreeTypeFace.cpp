#include <pch.h>

#include "FreeTypeFace.h"

#include <Core/GuGuFile.h>
#include <Core/FileSystem/FileSystem.h>

#include "FreeTypeUtils.h"

namespace GuGu {
	FreeTypeFace::FreeTypeFace(FT_Library* library, GuGuUtf8Str& filePath, std::shared_ptr<FileSystem> fileSystem)
		: m_library(library)
		, m_filePath(filePath)
		, m_ftStreamHandler(filePath, fileSystem)
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

	FT_Pos FreeTypeFace::getDescender() const
	{
		return FreeTypeUtils::getDescender(m_ftFace);
	}

	FT_Pos FreeTypeFace::getScaledHeight() const
	{
		return FreeTypeUtils::getScaledHeight(m_ftFace);
	}

    //FreeTypeFace::FreeTypeFace() {
//
    //}

    FreeTypeFace::FFTStreamHandler::FFTStreamHandler()
	{
	}
	FreeTypeFace::FFTStreamHandler::FFTStreamHandler(const GuGuUtf8Str& inFileName, std::shared_ptr<FileSystem> fileSystem)
	{
		m_fileSystem = fileSystem;

		m_fileSystem->OpenFile(inFileName, GuGuFile::FileMode::OnlyRead);
		m_fileSize = m_fileSystem->getFileSize();
	}
	FreeTypeFace::FFTStreamHandler::~FFTStreamHandler()
	{
	}
	void FreeTypeFace::FFTStreamHandler::CloseFile(FT_Stream inStream)
	{
		FFTStreamHandler* myStreamHandler = (FFTStreamHandler*)inStream->descriptor.pointer;
		if (myStreamHandler->m_fileSystem) //todo:may be need to clean
		{
			myStreamHandler->m_fileSystem->CloseFile();
		}
	}
	unsigned long FreeTypeFace::FFTStreamHandler::ReadData(FT_Stream inStream, unsigned long inOffset, unsigned char* inBuffer, unsigned long inCount)
	{
		FFTStreamHandler* myStreamHandler = (FFTStreamHandler*)inStream->descriptor.pointer;
		if (myStreamHandler->m_fileSystem)
		{
			if (!myStreamHandler->m_fileSystem->Seek(inOffset))
			{
				return 0;
			}

		}
		if (inCount > 0)
		{
			if (myStreamHandler->m_fileSystem)
			{
				int32_t bytesHaveReaded = 0;
				if (!myStreamHandler->m_fileSystem->ReadFile(inBuffer, inCount, bytesHaveReaded))
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		return inCount;
	}
}