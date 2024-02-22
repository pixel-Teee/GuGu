#pragma once

#include <vector>

#include "GuGuFile.h"

namespace GuGu {
	//void testArchiver();
	class GuGuUtf8Str;
	//class GuGuFile;
	void CreateArchive(const std::vector<GuGuUtf8Str>& filePaths, const GuGuUtf8Str& archiveName);

	void InitArchive();

	bool ReadArchive(const GuGuUtf8Str& filePath, std::vector<uint8_t>& buffer);

	bool ReadArchive(std::shared_ptr<GuGuFile> archiver, const GuGuUtf8Str& filePath, void* buffer, int32_t numberOfBytesToRead);

	int32_t getFileSize(const GuGuUtf8Str& filePath);

	bool Seek(std::shared_ptr<GuGuFile> archiver, GuGuUtf8Str& filePath, int32_t offset, GuGuFile::SeekDir seekDir);
}