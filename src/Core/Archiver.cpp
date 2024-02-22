#include <pch.h>

#include "Archiver.h"

#include <unordered_map>

namespace GuGu {
	void write(std::shared_ptr<GuGuFile> out, int32_t a)
	{
		char str[4];
		str[0] = static_cast<char>((a & 0x000000ff) >> 0);
		str[1] = static_cast<char>((a & 0x0000ff00) >> 8);
		str[2] = static_cast<char>((a & 0x00ff0000) >> 16);
		str[3] = static_cast<char>((a & 0xff000000) >> 24);
		out->WriteFile(str, 4);
	}

	int32_t getInt(std::shared_ptr<GuGuFile> in) {
		unsigned char buffer[4];
		int32_t numberOfBytesHaveReaded = 0;
		in->ReadFile(buffer, 4, numberOfBytesHaveReaded);
		int32_t r = buffer[0];
		r |= (buffer[1] << 8);
		r |= (buffer[2] << 16);
		r |= (buffer[3] << 24);
		return r;
	}

	struct Entry {
		int32_t mPosition;
		int32_t mSize;
	};

	std::unordered_map<GuGuUtf8Str, Entry> gEntries;

	void CreateArchive(const std::vector<GuGuUtf8Str>& fileNames, const GuGuUtf8Str& archiveName)
	{
		std::shared_ptr<GuGuFile> out = CreateFileFactory();
		out->OpenFile(archiveName, GuGuFile::OnlyWrite);

		int32_t* fileSizes = new int32_t[fileNames.size()];

		for (int32_t i = 0; i < fileNames.size(); ++i)
		{
			std::shared_ptr<GuGuFile> in = CreateFileFactory();
			in->OpenFile(fileNames[i], GuGuFile::OnlyRead);
			fileSizes[i] = in->getFileSize();
			char* data = new char[fileSizes[i]];
			int32_t numberOfBytesHaveReaded = 0;
			in->ReadFile(data, fileSizes[i], numberOfBytesHaveReaded);
			in->CloseFile();
			out->WriteFile(data, fileSizes[i]);
			delete[] data;
		}
		int32_t dataEnd = out->getCurrentFilePointerPos();
		write(out, fileNames.size());

		int32_t pos = 0;
		for (int32_t i = 0; i < fileNames.size(); ++i)
		{
			write(out, pos);
			write(out, fileSizes[i]);
			int32_t nameLength = fileNames[i].getTotalByteCount();
			write(out, nameLength);
			out->WriteFile((void*)fileNames[i].getStr(), nameLength);
			GuGu_LOGD("%d %d %d %s\n", pos, fileSizes[i], nameLength, fileNames[i].getStr());
			pos += fileSizes[i];
		}
		
		write(out, dataEnd);
		out->CloseFile();
		delete[] fileSizes;
		fileSizes = nullptr;
	}
	bool ReadArchive(const GuGuUtf8Str& filePath, std::vector<uint8_t>& buffer)
	{
		std::shared_ptr<GuGuFile> file = CreateFileFactory();
		file->OpenFile("archiver.bin", GuGuFile::OnlyRead);

		for (auto it : gEntries)
		{
			if (it.first == filePath)
			{
				const Entry& entry = it.second;
				buffer.resize(entry.mSize);
				file->Seek(entry.mPosition, GuGuFile::Begin);
				int32_t numberOfBytesHaveReaded = 0;
				file->ReadFile(buffer.data(), entry.mSize, numberOfBytesHaveReaded);
			}		
		}

		file->CloseFile();

		return true;
	}
	bool ReadArchive(std::shared_ptr<GuGuFile> archiver, const GuGuUtf8Str& filePath, void* buffer, int32_t numberOfBytesToRead)
	{
		//std::shared_ptr<GuGuFile> file = CreateFileFactory();

		for (auto it : gEntries)
		{
			if (it.first == filePath)
			{
				const Entry& entry = it.second;			
				int32_t numberOfBytesHaveReaded = 0;
				archiver->ReadFile(buffer, numberOfBytesToRead, numberOfBytesHaveReaded);
			}
		}

		//file->CloseFile();

		return true;
	}
	void InitArchive()
	{
		//init archive
		std::shared_ptr<GuGuFile> file = CreateFileFactory();
		file->OpenFile("archiver.bin", GuGuFile::OnlyRead);
		file->Seek(-4, GuGuFile::End);
		int32_t tableBegin = getInt(file);
		file->Seek(tableBegin, GuGuFile::Begin);
		int32_t fileNumber = getInt(file);
		for (int32_t i = 0; i < fileNumber; ++i)
		{
			Entry entry;
			entry.mPosition = getInt(file);
			entry.mSize = getInt(file);	
			int32_t nameTotalByteCount = getInt(file);
			char* name = new char[nameTotalByteCount + 1];
			int32_t haveReadedNumber = 0;
			file->ReadFile(name, nameTotalByteCount, haveReadedNumber);
			name[nameTotalByteCount] = '\0';
			gEntries.insert(std::make_pair(name, entry));
			delete []name;
		}
		file->CloseFile();
	}

	int32_t getFileSize(const GuGuUtf8Str& filePath)
	{
		for (auto it : gEntries)
		{
			if (it.first == filePath)
			{
				const Entry& entry = it.second;
				return entry.mSize;
			}
		}
		return 0;
	}

	bool Seek(std::shared_ptr<GuGuFile> archiver, GuGuUtf8Str& filePath, int32_t offset, GuGuFile::SeekDir seekDir)
	{
		for (auto it : gEntries)
		{
			if (it.first == filePath)
			{
				const Entry& entry = it.second;
				if (seekDir == GuGuFile::SeekDir::Begin)
				{
					archiver->Seek(offset + entry.mPosition, GuGuFile::SeekDir::Begin);
					break;
				}
			}
		}
		return true;
	}

}