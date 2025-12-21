#pragma once

#include <Core/GuGuFile.h>
#include <Core/GuGuUtf8Str.h>

#include <unordered_map>

namespace GuGu
{
	class GuGuUtf8Str;
	class FileSystem
	{
	public:
		virtual ~FileSystem() = default;

		virtual void OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode) = 0;

		virtual void CloseFile() = 0;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) = 0;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) = 0;

		virtual bool Seek(int64_t offset, GuGuFile::SeekDir seekDir = GuGuFile::SeekDir::Begin) = 0;

		virtual int32_t getFileSize() = 0;

		virtual int32_t getCurrentFilePointerPos() = 0;

		virtual bool fileExists(const GuGuUtf8Str& filePath) = 0;

		virtual bool folderExists(const GuGuUtf8Str& folderPath) = 0;

		virtual void rename(const GuGuUtf8Str& oldPath, const GuGuUtf8Str& newPath) = 0;
	};
	
	class NativeFileSystem : public FileSystem
	{
	public:
		NativeFileSystem(const GuGuUtf8Str& nativePath);

		virtual ~NativeFileSystem() = default;

		virtual void OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode) override;

		virtual void CloseFile() override;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) override;

		virtual bool Seek(int64_t offset, GuGuFile::SeekDir seekDir = GuGuFile::SeekDir::Begin) override;

		virtual int32_t getFileSize() override;

		virtual int32_t getCurrentFilePointerPos() override;

		GuGuUtf8Str getNativeFilePath() const;

		virtual bool fileExists(const GuGuUtf8Str& filePath) override;

		virtual bool folderExists(const GuGuUtf8Str& folderPath) override;

		virtual void rename(const GuGuUtf8Str& oldPath, const GuGuUtf8Str& newPath) override;
	private:
		std::shared_ptr<GuGuFile> m_file;

		GuGuUtf8Str m_nativePath;
	};

	class ArchiverFileSystem : public FileSystem
	{
	public:
		struct Entry {
			int32_t mPosition;
			int32_t mSize;
		};
		ArchiverFileSystem(const GuGuUtf8Str& archiverNativePath);

		virtual ~ArchiverFileSystem() = default;

		virtual void OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode) override;

		virtual void CloseFile() override;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) override;

		virtual bool Seek(int64_t offset, GuGuFile::SeekDir seekDir = GuGuFile::SeekDir::Begin) override;

		virtual int32_t getFileSize() override;

		virtual int32_t getCurrentFilePointerPos() override;

		virtual bool fileExists(const GuGuUtf8Str& filePath) override;

		virtual bool folderExists(const GuGuUtf8Str& folderPath) override;

		virtual void rename(const GuGuUtf8Str& oldPath, const GuGuUtf8Str& newPath) override;
	private:
		std::shared_ptr<FileSystem> m_underlyingFileSystem;

		std::unordered_map<GuGuUtf8Str, Entry> m_entries;

		GuGuUtf8Str m_archiverNativePath;

		GuGuUtf8Str m_currentOpenAssetPath;
	};

	class RootFileSystem : public FileSystem
	{
	public:
		RootFileSystem();

		virtual ~RootFileSystem() = default;

	public:
		void mount(const GuGuUtf8Str& path, std::shared_ptr<FileSystem> fs);
		void mount(const GuGuUtf8Str& path, const GuGuUtf8Str& nativePath);
		bool unmount(const GuGuUtf8Str& path);

		virtual void OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode) override;

		virtual void CloseFile() override;

		virtual void WriteFile(void* buffer, int32_t numberOfBytesToWrite) override;

		virtual int32_t ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded) override;

		virtual bool Seek(int64_t offset, GuGuFile::SeekDir seekDir = GuGuFile::SeekDir::Begin) override;

		virtual int32_t getFileSize() override;

		virtual int32_t getCurrentFilePointerPos() override;

		GuGuUtf8Str findMountPoint(std::shared_ptr<FileSystem> fileSystem) const;

		bool findMountPoint(const GuGuUtf8Str& path, GuGuUtf8Str* relativePath, std::shared_ptr<FileSystem>& pFs);

		virtual bool fileExists(const GuGuUtf8Str& filePath) override;

		virtual bool folderExists(const GuGuUtf8Str& folderPath) override;

		virtual void rename(const GuGuUtf8Str& oldPath, const GuGuUtf8Str& newPath) override;
	private:
		std::vector<std::pair<GuGuUtf8Str, std::shared_ptr<FileSystem>>> m_mountPoints;

		std::shared_ptr<FileSystem> m_currentOpenFileSystem;
	};

	void CreateArchiveFiles(std::shared_ptr<RootFileSystem> archiveRootFileSystem, 
		std::shared_ptr<RootFileSystem> assetRootFileSystem,
		const std::vector<GuGuUtf8Str>& fileVirtualPaths, const GuGuUtf8Str& archiveVirtualPaths, const GuGuUtf8Str& mountPoint);

	std::vector <uint8_t> ReadFile(const GuGuUtf8Str& path, std::shared_ptr<RootFileSystem> rootFileSystem);
}