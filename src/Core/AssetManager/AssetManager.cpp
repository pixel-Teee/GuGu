#include <pch.h>

#include "AssetManager.h"

#include <Application/Application.h>
#include <Core/FileSystem/FilePath.h>

#ifdef WIN32 
#include <windows.h>
#endif

namespace GuGu {
	AssetManager::AssetManager()
	{
		GuGuUtf8Str contentDirectory = Application::GetContentDirectoryWithExecutable();

		m_nativeFileSystem = std::make_shared<NativeFileSystem>(contentDirectory);
		m_rootFileSystem = std::make_shared<RootFileSystem>();
		m_rootFileSystem->mount("content", m_nativeFileSystem);

		if (!m_nativeFileSystem->fileExists(m_nativeFileSystem->getNativeFilePath() + "/AssetRgistry.json"))
		{
			//create this file
			m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyWrite);
			nlohmann::json newAssetRegistry = nlohmann::json::object();
			newAssetRegistry["AssetRegistry"] = nlohmann::json::array();
			GuGuUtf8Str data = newAssetRegistry.dump();
			m_rootFileSystem->WriteFile((void*)data.getStr(), data.getTotalByteCount());
			m_rootFileSystem->CloseFile();
		}
		//加载这个文件，这个文件存储了文件路径和其对应的GUID
		m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyRead);
		int32_t fileSize = m_rootFileSystem->getFileSize();
		char* data = new char[fileSize + 1];
		data[fileSize] = '\0';
		int32_t numberOfBytesHaveReaded = 0;
		m_rootFileSystem->ReadFile(data, fileSize, numberOfBytesHaveReaded);
		m_rootFileSystem->CloseFile();
		//fileContent to GuGuUtfStr
		GuGuUtf8Str fileContent(data);
		delete[] data;
		m_assetRegistryJson = nlohmann::json::parse(fileContent.getStr());
		nlohmann::json root = m_assetRegistryJson["AssetRegistry"];
		for (const auto& item : root)
		{
			GGuid key = GuGuUtf8Str(item.at("GUID").get<std::string>());
			GuGuUtf8Str filePath = item.at("FilePath").get<std::string>();
			GuGuUtf8Str fileName = item.at("FileName").get<std::string>();
			uint32_t assetTypeId = item.at("AssetType").get<uint32_t>();
			m_guidToAssetMap.insert({ key, {filePath, fileName, meta::Type(assetTypeId)}});
		}
	}
	AssetManager::~AssetManager()
	{
	}
	AssetManager& AssetManager::getAssetManager()
	{
		static AssetManager g_assetManager;
		return g_assetManager;
	}
	void AssetManager::getSubPaths(const GuGuUtf8Str& relativePath, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		//traverseDirectoryAndFile_private(m_nativeFileSystem->getNativeFilePath() + "/" + relativePath, enumerateCallBack);
		GuGuUtf8Str directory;
		if (relativePath != "")
		{
			directory = m_nativeFileSystem->getNativeFilePath() + "/" + relativePath;
		}
		else
		{
			directory = m_nativeFileSystem->getNativeFilePath();
		}
		
#ifdef WIN32 
		std::wstring searchDirectory = (directory + "/*").getUtf16String();

		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(searchDirectory.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGE("%s", "error finding first file in directory");
			return;
		}

		do {
			const std::wstring& fileName = findData.cFileName;
			if (fileName == L"." || fileName == L"..")
				continue;
			std::wstring fullPath = directory.getUtf16String() + L"/" + fileName;
			GuGuUtf8Str utf8FullPath = GuGuUtf8Str::fromUtf16ToUtf8(fullPath.c_str());
			GuGuUtf8Str utf8RelativePath = FilePath::getRelativePath(utf8FullPath + "/", m_nativeFileSystem->getNativeFilePath() + "/").getStr();

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				GuGu_LOGD("directory %s\n", utf8RelativePath.getStr());
				if (enumerateCallBack)
				{
					bool isDirectory = true;
					//将绝对路径转换为相对路径
					enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				}
			}
			else
			{
				bool isDirectory = false; //isFile
				enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				GuGu_LOGE("File: %s\n", utf8RelativePath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}
	void AssetManager::traverseDirectoryAndFile(std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		traverseDirectoryAndFile_private(m_nativeFileSystem->getNativeFilePath(), enumerateCallBack);
	}

	GuGuUtf8Str AssetManager::getRootPath() const
	{
		//return m_nativeFileSystem->getNativeFilePath();
		return m_rootFileSystem->findMountPoint(m_nativeFileSystem);
	}

	GuGuUtf8Str AssetManager::getActualPhysicalPath(const GuGuUtf8Str& relativePath)
	{
		if (relativePath != "")
		{
			return m_nativeFileSystem->getNativeFilePath() + "/" + relativePath;
		}
		else
		{
			return m_nativeFileSystem->getNativeFilePath();
		}
	}

	std::shared_ptr<RootFileSystem> AssetManager::getRootFileSystem() const
	{
		return m_rootFileSystem;
	}

	void AssetManager::registerAsset(const GuGuUtf8Str& guid, const GuGuUtf8Str& filePath, const GuGuUtf8Str& fileName, meta::Type assetType)
	{
		//GuGuUtf8Str relativePath = "content/" + FilePath::getRelativePathForAsset(filePath, m_nativeFileSystem->getNativeFilePath() + "/").getStr();
		GuGuUtf8Str relativePath = filePath;
		m_guidToAssetMap.insert({ guid, {relativePath, fileName, assetType.GetID()} });
		m_rootFileSystem->OpenFile("content/AssetRgistry.json", GuGuFile::FileMode::OnlyWrite);
		
		nlohmann::json newItem = nlohmann::json::object();
		newItem["GUID"] = guid.getStr();
		newItem["FilePath"] = relativePath.getStr();
		newItem["FileName"] = fileName.getStr();
		newItem["AssetType"] = assetType.GetID();
		m_assetRegistryJson["AssetRegistry"].push_back(newItem);
		GuGuUtf8Str jsonFileContent = m_assetRegistryJson.dump();
		m_rootFileSystem->WriteFile((void*)jsonFileContent.getStr(), jsonFileContent.getTotalByteCount());
		m_rootFileSystem->CloseFile();
	}

	bool AssetManager::isInAssetRegistry(const GGuid& fileGuid) const
	{
		if (m_guidToAssetMap.find(fileGuid) == m_guidToAssetMap.end())
			return false;
		return true;
	}

	bool AssetManager::isInAssetRegistry(const GuGuUtf8Str& filePath) const
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second.m_filePath == filePath)
				return true;
		}
		return false;
	}

	const AssetData& AssetManager::getAssetData(const GuGuUtf8Str& filePath) const
	{
		for (const auto& item : m_guidToAssetMap)
		{
			if (item.second.m_filePath == filePath)
				return item.second;
		}
		return AssetData();
	}

	//遍历目录
	void AssetManager::traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
#ifdef WIN32 
		std::wstring searchDirectory = (directory + "/*").getUtf16String();

		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile(searchDirectory.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			GuGu_LOGE("%s", "error finding first file in directory");
			return;
		}

		do {
			const std::wstring& fileName = findData.cFileName;
			if(fileName == L"." || fileName == L"..")
				continue;
			std::wstring fullPath = directory.getUtf16String() + L"/" + fileName;
			GuGuUtf8Str utf8FullPath = GuGuUtf8Str::fromUtf16ToUtf8(fullPath.c_str());
			GuGuUtf8Str utf8RelativePath = FilePath::getRelativePath(utf8FullPath + "/", m_nativeFileSystem->getNativeFilePath() + "/").getStr();

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{	
				GuGu_LOGD("directory %s\n", utf8RelativePath.getStr());
				if (enumerateCallBack)
				{
					bool isDirectory = true;
					//将绝对路径转换为相对路径
					enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				}
				traverseDirectoryAndFile_private(utf8FullPath, enumerateCallBack);
			}
			else
			{
				bool isDirectory = false; //isFile
				enumerateCallBack("content/" + utf8RelativePath, isDirectory);
				GuGu_LOGD("File: %s\n", utf8RelativePath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}

	
}