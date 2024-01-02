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
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_READ, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		case FileMode::OnlyWrite:
			m_fileHandle = CreateFile(path.getUtf16String().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
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
		LPDWORD tmpValue = 0;
		::WriteFile(m_fileHandle, buffer, numberOfBytesToWrite, tmpValue, nullptr);
		numberOfBytesWereWritten = *tmpValue;
	}
	int32_t WindowsGuGuFile::ReadFile(void* buffer, int32_t numberOfBytesToRead)
	{
		int32_t numberOfBytesWereRead = 0;
		LPDWORD tmpValue = 0;
		::ReadFile(m_fileHandle, buffer, numberOfBytesToRead, tmpValue, nullptr);
		numberOfBytesWereRead = *tmpValue;
		return numberOfBytesToRead;
	}
	int32_t WindowsGuGuFile::getFileSize()
	{
		LARGE_INTEGER info;
		memset(&info, 0, sizeof(info));
		GetFileSizeEx(m_fileHandle, &info);
		return info.QuadPart;
	}
}