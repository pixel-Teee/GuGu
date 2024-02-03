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
			std::wstring filePath = path.getUtf16String();
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		}
		case FileMode::OnlyWrite:
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		}

		if (m_fileHandle == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGD("open file error");
			m_fileHandle = NULL;
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
	int32_t WindowsGuGuFile::getFileSize()
	{
		LARGE_INTEGER info;
		memset(&info, 0, sizeof(info));
		GetFileSizeEx(m_fileHandle, &info);
		return info.QuadPart;
	}
}