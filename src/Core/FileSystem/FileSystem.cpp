#include <pch.h>

#include "FileSystem.h"

namespace GuGu {
	//------native file system------
	NativeFileSystem::NativeFileSystem(const GuGuUtf8Str& nativePath)
	{
		m_nativePath = nativePath;
		m_file = CreateFileFactory();
	}

	void NativeFileSystem::OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode)
	{
		GuGuUtf8Str nativePath;
		if(m_nativePath == "")
			nativePath = path;
		else
			nativePath = m_nativePath + "/" + path;
		m_file->OpenFile(nativePath, fileMode);
	}

	void NativeFileSystem::CloseFile()
	{
		m_file->CloseFile();
	}

	void NativeFileSystem::WriteFile(void* buffer, int32_t numberOfBytesToWrite)
	{
		m_file->WriteFile(buffer, numberOfBytesToWrite);
	}

	int32_t NativeFileSystem::ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded)
	{
		return m_file->ReadFile(buffer, numberOfBytesToRead, numberOfBytesHaveReaded);
	}

	bool NativeFileSystem::Seek(int64_t offset, GuGuFile::SeekDir seekDir)
	{
		return m_file->Seek(offset, seekDir);
	}

	int32_t NativeFileSystem::getFileSize()
	{
		return m_file->getFileSize();
	}

	int32_t NativeFileSystem::getCurrentFilePointerPos()
	{
		return m_file->getCurrentFilePointerPos();
	}

	GuGuUtf8Str NativeFileSystem::getNativeFilePath() const
	{
		return m_nativePath;
	}

	//------archiver file system------
	int32_t getInt(std::shared_ptr<FileSystem> in) {
		unsigned char buffer[4];
		int32_t numberOfBytesHaveReaded = 0;
		in->ReadFile(buffer, 4, numberOfBytesHaveReaded);
		int32_t r = buffer[0];
		r |= (buffer[1] << 8);
		r |= (buffer[2] << 16);
		r |= (buffer[3] << 24);
		return r;
	}
	void write(std::shared_ptr<FileSystem> out, int32_t a)
	{
		char str[4];
		str[0] = static_cast<char>((a & 0x000000ff) >> 0);
		str[1] = static_cast<char>((a & 0x0000ff00) >> 8);
		str[2] = static_cast<char>((a & 0x00ff0000) >> 16);
		str[3] = static_cast<char>((a & 0xff000000) >> 24);
		out->WriteFile(str, 4);
	}
	ArchiverFileSystem::ArchiverFileSystem(const GuGuUtf8Str& archiverNativePath)
	{
		m_archiverNativePath = archiverNativePath;

		//------init archiver entries------
		m_underlyingFileSystem = std::make_shared<NativeFileSystem>("");
		m_underlyingFileSystem->OpenFile(m_archiverNativePath, GuGuFile::OnlyRead);
		m_underlyingFileSystem->Seek(-4, GuGuFile::End);
		int32_t tableBegin = getInt(m_underlyingFileSystem);
		m_underlyingFileSystem->Seek(tableBegin, GuGuFile::Begin);
		int32_t fileNumber = getInt(m_underlyingFileSystem);
		for (int32_t i = 0; i < fileNumber; ++i)
		{
			Entry entry;
			entry.mPosition = getInt(m_underlyingFileSystem);
			entry.mSize = getInt(m_underlyingFileSystem);
			int32_t assetPathTotalByteCount = getInt(m_underlyingFileSystem);
			char* assetPath = new char[assetPathTotalByteCount + 1];
			int32_t haveReadedNumber = 0;
			m_underlyingFileSystem->ReadFile(assetPath, assetPathTotalByteCount, haveReadedNumber);
			assetPath[assetPathTotalByteCount] = '\0';
			m_entries.insert(std::make_pair(assetPath, entry));
			GuGu_LOGD("%s %d %d\n", assetPath, entry.mPosition, entry.mSize);
			delete[]assetPath;
		}
		m_underlyingFileSystem->CloseFile();
		//------init archiver entries------
	}
	void ArchiverFileSystem::OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode)//this path is inside archiver
	{
		GuGuUtf8Str assetFilePath = path;
		if (m_entries.find(assetFilePath) == m_entries.end())
			GuGu_LOGE("could not find asset in archiver file");
		else
		{
			m_underlyingFileSystem->OpenFile(m_archiverNativePath, fileMode);

			//to read asset file path
			m_currentOpenAssetPath = assetFilePath;

			Seek(0, GuGuFile::SeekDir::Begin);//note:go to the asset position
		}		
	}
	void ArchiverFileSystem::CloseFile()
	{
		m_underlyingFileSystem->CloseFile();
	}
	void ArchiverFileSystem::WriteFile(void* buffer, int32_t numberOfBytesToWrite)
	{
		//not implement
	}
	int32_t ArchiverFileSystem::ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded)
	{
		for (auto it : m_entries)
		{
			if (it.first == m_currentOpenAssetPath)
			{
				const Entry& entry = it.second;
				int32_t numberOfBytesHaveReaded = 0;
				//m_underlyingFileSystem->Seek(entry.mPosition, GuGuFile::SeekDir::Begin);
				return m_underlyingFileSystem->ReadFile(buffer, numberOfBytesToRead, numberOfBytesHaveReaded);
			}
		}
		return false;//todo:fix this return type to bool type
	}
	bool ArchiverFileSystem::Seek(int64_t offset, GuGuFile::SeekDir seekDir)
	{
		for (auto it : m_entries)
		{
			if (it.first == m_currentOpenAssetPath)
			{
				const Entry& entry = it.second;
				if (seekDir == GuGuFile::SeekDir::Begin)
				{
					m_underlyingFileSystem->Seek(offset + entry.mPosition, GuGuFile::SeekDir::Begin);
					break;
				}
			}
		}
		return true;
	}
	int32_t ArchiverFileSystem::getFileSize()
	{
		for (auto it : m_entries)
		{
			if (it.first == m_currentOpenAssetPath)
			{
				const Entry& entry = it.second;
				return entry.mSize;
			}
		}
		return 0;
	}
	int32_t ArchiverFileSystem::getCurrentFilePointerPos()
	{
		for (auto it : m_entries)
		{
			if (it.first == m_currentOpenAssetPath)
			{
				const Entry& entry = it.second;
				return m_underlyingFileSystem->getCurrentFilePointerPos() - entry.mPosition;//relative to asset begin pos
			}
		}
		return 0;
	}

	//------root file system------
	RootFileSystem::RootFileSystem()
	{
	}

	void RootFileSystem::mount(const GuGuUtf8Str& path, std::shared_ptr<FileSystem> fs)
	{
		std::shared_ptr<FileSystem> fileSystem;
		if (findMountPoint(path, nullptr, fileSystem))
		{
			GuGu_LOGE("Cannot mount a filesystem at %s: there is another FS that includes this path", path.getStr());
			return;
		}
		m_mountPoints.push_back(std::make_pair(path, fs));
	}

	void RootFileSystem::mount(const GuGuUtf8Str& path, const GuGuUtf8Str& nativePath)
	{
		//mount(path, std::make_shared<NativeFileSystem>())
	}

	bool RootFileSystem::unmount(const GuGuUtf8Str& path)
	{
		for (size_t index = 0; index < m_mountPoints.size(); ++index)
		{
			if (m_mountPoints[index].first == path)
			{
				m_mountPoints.erase(m_mountPoints.begin() + index);
				return true;
			}
		}
		return false;
	}

	void RootFileSystem::OpenFile(const GuGuUtf8Str& path, GuGuFile::FileMode fileMode)
	{
		GuGuUtf8Str relativePath;
		std::shared_ptr<FileSystem> pFs;

		if (findMountPoint(path, &relativePath, pFs))
		{
			pFs->OpenFile(relativePath, fileMode);
		}
		m_currentOpenFileSystem = pFs;
	}

	void RootFileSystem::CloseFile()
	{
		m_currentOpenFileSystem->CloseFile();
		m_currentOpenFileSystem = nullptr;
	}

	void RootFileSystem::WriteFile(void* buffer, int32_t numberOfBytesToWrite)
	{
		m_currentOpenFileSystem->WriteFile(buffer, numberOfBytesToWrite);
	}

	int32_t RootFileSystem::ReadFile(void* buffer, int32_t numberOfBytesToRead, int32_t& numberOfBytesHaveReaded)
	{
		return m_currentOpenFileSystem->ReadFile(buffer, numberOfBytesToRead, numberOfBytesHaveReaded);
	}

	bool RootFileSystem::Seek(int64_t offset, GuGuFile::SeekDir seekDir)
	{
		return m_currentOpenFileSystem->Seek(offset, seekDir);
	}

	int32_t RootFileSystem::getFileSize()
	{
		return m_currentOpenFileSystem->getFileSize();
	}

	int32_t RootFileSystem::getCurrentFilePointerPos()
	{
		return m_currentOpenFileSystem->getCurrentFilePointerPos();
	}

	GuGuUtf8Str RootFileSystem::findMountPoint(std::shared_ptr<FileSystem> fileSystem) const
	{
		for (auto it : m_mountPoints)
		{
			if (it.second == fileSystem)
			{
				return it.first;
			}
		}
		return "";
	}

	bool RootFileSystem::findMountPoint(const GuGuUtf8Str& path, GuGuUtf8Str* relativePath, std::shared_ptr<FileSystem>& pFs)
	{
		for (auto it : m_mountPoints)
		{
			//1:virtual path need to at begin
			if (path.find(it.first.getStr(), 0) == 0 && ((path.len() == it.first.len()) || (path[it.first.len()] == GuGuUtf8Str("/"))))
			{
				if (relativePath)
				{
					GuGuUtf8Str relative = (path.len() == it.first.len()) ? "" : path.substr(it.first.len() + 1);
					*relativePath = relative;
				}

				//if (pFs)
				//{
				pFs = it.second;
				//}

				return true;
			}
		}

		return false;
	}

	void CreateArchiveFiles(std::shared_ptr<RootFileSystem> rootFileSystem, std::shared_ptr<RootFileSystem> assetRootFileSystem, const std::vector<GuGuUtf8Str>& fileVirtualPaths, const GuGuUtf8Str& archiveVirtualPaths, const GuGuUtf8Str& mountPoint)
	{
		rootFileSystem->OpenFile(archiveVirtualPaths, GuGuFile::OnlyWrite);
		int32_t* fileSizes = new int32_t[fileVirtualPaths.size()];

		for (int32_t i = 0; i < fileVirtualPaths.size(); ++i)
		{
			assetRootFileSystem->OpenFile(mountPoint + "/" + fileVirtualPaths[i], GuGuFile::OnlyRead);
			fileSizes[i] = assetRootFileSystem->getFileSize();
			char* data = new char[fileSizes[i]];
			int32_t numberOfBytesHaveReaded = 0;
			assetRootFileSystem->ReadFile(data, fileSizes[i], numberOfBytesHaveReaded);
			assetRootFileSystem->CloseFile();
			rootFileSystem->WriteFile(data, fileSizes[i]);
			delete[] data;
		}
		int32_t dataEnd = rootFileSystem->getCurrentFilePointerPos();
		write(rootFileSystem, fileVirtualPaths.size());

		int32_t pos = 0;
		for (int32_t i = 0; i < fileVirtualPaths.size(); ++i)
		{
			write(rootFileSystem, pos);
			write(rootFileSystem, fileSizes[i]);
			int32_t assetFilePathLengh = fileVirtualPaths[i].getTotalByteCount();
			write(rootFileSystem, assetFilePathLengh);
			rootFileSystem->WriteFile((void*)fileVirtualPaths[i].getStr(), assetFilePathLengh);
			GuGu_LOGD("%d %d %d %s\n", pos, fileSizes[i], assetFilePathLengh, fileVirtualPaths[i].getStr());
			pos += fileSizes[i];
		}

		write(rootFileSystem, dataEnd);
		rootFileSystem->CloseFile();
		delete[] fileSizes;
		fileSizes = nullptr;
	}

	std::vector <uint8_t> ReadFile(const GuGuUtf8Str& path, std::shared_ptr<RootFileSystem> rootFileSystem) {

		std::vector<uint8_t> fileContent;
		rootFileSystem->OpenFile(path, GuGuFile::FileMode::OnlyRead);
		int32_t fileLength = rootFileSystem->getFileSize();
		fileContent.resize(fileLength);
		int32_t haveReadedLength = 0;
		rootFileSystem->ReadFile(fileContent.data(), fileLength, haveReadedLength);
		rootFileSystem->CloseFile();
		return fileContent;
	}
}