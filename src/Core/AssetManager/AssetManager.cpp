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
				GuGu_LOGE("File: %s\n", utf8RelativePath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}

	
}