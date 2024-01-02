#pragma once

#include <Core/GuGuFile.h>

namespace GuGu {
	class WindowsGuGuFile : public GuGuFile{
	public:
		WindowsGuGuFile();

		virtual ~WindowsGuGuFile();

		virtual void OpenFile(const GuGuUtf8Str& path, FileMode fileMode) override;

		virtual void CloseFile() override;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead) override;

		virtual int32_t getFileSize() override;
	private:

		HANDLE m_fileHandle;
	};
}