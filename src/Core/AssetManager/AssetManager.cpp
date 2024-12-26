#include <pch.h>

#include "AssetManager.h"

#include <Application/Application.h>

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
	void AssetManager::traverseDirectoryAndFile(std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
		traverseDirectoryAndFile_private(m_nativeFileSystem->getNativeFilePath(), enumerateCallBack);
	}
	//遍历目录
	void AssetManager::traverseDirectoryAndFile_private(const GuGuUtf8Str& directory, std::function<void(GuGuUtf8Str, bool)> enumerateCallBack)
	{
#ifdef WIN32 
		std::wstring searchDirectory = (directory + "\*").getUtf16String();

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
			std::wstring fullPath = directory.getUtf16String() + L"\\" + fileName;
			GuGuUtf8Str utf8FullPath = GuGuUtf8Str::fromUtf16ToUtf8(fullPath.c_str());
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{	
				GuGu_LOGD("directory %s\n", utf8FullPath.getStr());
				if (enumerateCallBack)
				{
					bool isDirectory = true;
					enumerateCallBack(utf8FullPath, isDirectory);
				}
				traverseDirectoryAndFile_private(utf8FullPath, enumerateCallBack);
			}
			else
			{
				bool isDirectory = false; //isFile
				enumerateCallBack(utf8FullPath, isDirectory);
				GuGu_LOGE("File: %s\n", utf8FullPath.getStr());
			}
		} while (FindNextFile(hFind, &findData) != 0);

		FindClose(hFind);
#endif
	}

	
}