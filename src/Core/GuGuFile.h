#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class GuGuFile {
	public:
		GuGuFile();

		virtual ~GuGuFile();

        enum FileMode{
            OnlyRead,
            OnlyWrite,
			AppendWrite
        };

		enum SeekDir
		{
			Begin,
			End,
			Current
		};

        virtual void OpenFile(const GuGuUtf8Str& path, FileMode fileMode) = 0;

		virtual void CloseFile() = 0;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) = 0;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) = 0;

		virtual bool Seek(int64_t offset, SeekDir seekDir = Begin) = 0;

		virtual int32_t getFileSize() = 0;

		virtual int32_t getCurrentFilePointerPos() = 0;

		virtual bool fileExists(const GuGuUtf8Str& filePath) = 0;

		virtual bool folderExists(const GuGuUtf8Str& folderPath) = 0;
	};

	std::shared_ptr<GuGuFile> CreateFileFactory();
}