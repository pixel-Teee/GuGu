#include <pch.h>

#include "FilePath.h"

#ifdef WIN32
	#include <windows.h>
#endif

namespace GuGu {
	static void splitPath(const GuGuUtf8Str& inPath, std::vector<GuGuUtf8Str>& splitPath)
	{
		int32_t start = 0;
		int32_t dirStep = 0;
		do {
			dirStep = inPath.findFirstOf("\\/", start);
			if (dirStep == -1)
			{
				splitPath.push_back(inPath[start]);
			}
			else
			{
				splitPath.push_back(inPath.substr(start, dirStep - start));
				//splitPath.push_back()
			}
			start = dirStep + 1;
			if(start >= inPath.len())
				break;
		} while (dirStep != -1);
	}

	FilePath::FilePath()
	{
	}
	FilePath::FilePath(const GuGuUtf8Str& filePath)
		: m_filePath(filePath)
	{

	}
	FilePath::FilePath(GuGuUtf8Str&& filePath)
		: m_filePath(filePath)
	{
	}
	FilePath FilePath::getRelativePath(const FilePath& to, const FilePath& from)
	{
		//GuGuUtf8Str toDirPath;
		//GuGuUtf8Str toFromPath;
		//if (to.isFile())
		//{
		//	toDirPath = to.m_filePath.substr(0, to.m_filePath.findLastOf("\\/"));
		//}
		//if (from.isFile())
		//{
		//	toFromPath = from.m_filePath.substr(0, from.m_filePath.findLastOf("\\/"));
		//}

		//return FilePath();
		std::vector<GuGuUtf8Str> toDirs;
		std::vector<GuGuUtf8Str> fromDirs;
		splitPath(to.m_filePath, toDirs);
		splitPath(from.m_filePath, fromDirs);
		//split path
		GuGuUtf8Str output;

		std::vector<GuGuUtf8Str>::const_iterator toIt = toDirs.begin(),
			toEnd = toDirs.end(),
			fromIt = fromDirs.begin(),
			fromEnd = fromDirs.end();

		while (toIt != toEnd && fromIt != fromEnd && *toIt == *fromIt)
		{
			++toIt;
			++fromIt;
		}

		while (fromIt != fromEnd)
		{
			output += "../";
			++fromIt;
		}

		while (toIt != toEnd)
		{
			output += *toIt;
			++toIt;

			if (toIt != toEnd)
				output += "/";
		}
		return output;
	}
	GuGuUtf8Str FilePath::getBaseFileName(const GuGuUtf8Str& folderPath)
	{
		//获取一个文件夹路径的文件名
		std::vector<GuGuUtf8Str> folderDirs;
		splitPath(folderPath, folderDirs);
		if (folderDirs.size() > 0)
			return folderDirs.back();
		return "";
	}
	GuGuUtf8Str FilePath::getStr() const
	{
		return m_filePath;
	}
	bool FilePath::isDir() const
	{
#ifdef WIN32
		DWORD attributes = GetFileAttributes(m_filePath.getUtf16String().c_str());

		return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
		return false;
	}
	bool FilePath::isFile() const
	{
		return !isDir();
	}
}