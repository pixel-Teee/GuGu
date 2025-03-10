#include <pch.h>

#include "WindowsGuGuFile.h"

namespace GuGu {
	WindowsGuGuFile::WindowsGuGuFile()
	{
		m_fileHandle = nullptr;
	}
	WindowsGuGuFile::~WindowsGuGuFile()
	{
		m_fileHandle = nullptr;
	}
	void WindowsGuGuFile::OpenFile(const GuGuUtf8Str& path, FileMode fileMode)
	{
		switch (fileMode)
		{
		case FileMode::OnlyRead:
		{
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		}
		case FileMode::OnlyWrite: //覆盖写
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		case FileMode::AppendWrite: //追加写
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		}

		if (m_fileHandle == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGD("open file error");
			m_fileHandle = NULL;
		}
		if (fileMode == FileMode::AppendWrite)
		{
			SetFilePointer(m_fileHandle, 0, NULL, FILE_END);
		}
	}
	void WindowsGuGuFile::CloseFile()
	{
		CloseHandle(m_fileHandle);
	}
	void WindowsGuGuFile::WriteFile(void* buffer, int32_t numberOfBytesToWrite)
	{
		int32_t numberOfBytesWereWritten = 0;
		DWORD tmpValue = 0;
		::WriteFile(m_fileHandle, buffer, numberOfBytesToWrite, &tmpValue, nullptr);
		numberOfBytesWereWritten = tmpValue;
	}
	int32_t WindowsGuGuFile::ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded)
	{
		//int32_t numberOfBytesWereRead = 0;
		DWORD tmpValue = 0;
		bool retValue = ::ReadFile(m_fileHandle, buffer, numberOfBytesToRead, &tmpValue, nullptr);
		numberOfBytesHaveReaded = tmpValue;
		return retValue;
	}

	bool WindowsGuGuFile::Seek(int64_t offset, SeekDir seekDir)
	{
		if(seekDir == SeekDir::Begin)
			SetFilePointer(m_fileHandle, offset, NULL, FILE_BEGIN);
		else if(seekDir == SeekDir::Current)
			SetFilePointer(m_fileHandle, offset, NULL, FILE_CURRENT);
		else if(seekDir == SeekDir::End)
			SetFilePointer(m_fileHandle, offset, NULL, FILE_END);
		return true;
	}

	int32_t WindowsGuGuFile::getFileSize()
	{
		LARGE_INTEGER info;
		memset(&info, 0, sizeof(info));
		GetFileSizeEx(m_fileHandle, &info);
		return info.QuadPart;
	}

	int32_t WindowsGuGuFile::getCurrentFilePointerPos()
	{
		LARGE_INTEGER moveDistance;
		moveDistance.QuadPart = 0;

		LARGE_INTEGER currentPosition;
		currentPosition.QuadPart = 0;

		SetFilePointerEx(m_fileHandle, moveDistance, &currentPosition, FILE_CURRENT);

		return currentPosition.QuadPart;
	}

	bool WindowsGuGuFile::fileExists(const GuGuUtf8Str& filePath)
	{
		const std::wstring filePathUtf16 = filePath.getUtf16String();
		DWORD attributes = GetFileAttributes(filePathUtf16.c_str());
		return (attributes != INVALID_FILE_ATTRIBUTES);
	}

	bool WindowsGuGuFile::folderExists(const GuGuUtf8Str& folderPath)
	{
		const std::wstring folderPathUtf16 = folderPath.getUtf16String();
		DWORD attributes = GetFileAttributes(folderPathUtf16.c_str());
		if (attributes == INVALID_FILE_ATTRIBUTES) {
			// 路径不存在或者无法访问
			return false;
		}
		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	std::shared_ptr<GuGuFile> CreateFileFactory()
	{
		return std::make_shared<WindowsGuGuFile>();
	}
}