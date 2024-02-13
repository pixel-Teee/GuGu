#pragma once

#include <Core/GuGuFile.h>

#include <Windows.h>

namespace GuGu {
	class WindowsGuGuFile : public GuGuFile{
	public:
		WindowsGuGuFile();

		virtual ~WindowsGuGuFile();

		virtual void OpenFile(const GuGuUtf8Str& path, FileMode fileMode) override;

		virtual void CloseFile() override;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) override;

		virtual int32_t getFileSize() override;

		virtual bool Seek(uint64_t newPosition) override;
	private:

		HANDLE m_fileHandle;
	};
}