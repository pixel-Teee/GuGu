#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class FilePath
	{
	public:
		FilePath();

		FilePath(const GuGuUtf8Str& filePath);

		FilePath(GuGuUtf8Str&& filePath);

		//to 和 from 必须都是 directory
		static FilePath getRelativePath(const FilePath& to, const FilePath& from);

		static GuGuUtf8Str getBaseFileName(const GuGuUtf8Str& folderPath);

		GuGuUtf8Str getStr() const;

		bool isDir() const;

		bool isFile() const;
	private:
		GuGuUtf8Str m_filePath;//absolute or relative path
	};
}