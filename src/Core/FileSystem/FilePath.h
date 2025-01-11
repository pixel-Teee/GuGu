#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class FilePath
	{
	public:
		FilePath();

		FilePath(const GuGuUtf8Str& filePath);

		FilePath(GuGuUtf8Str&& filePath);

		//to 是资产路径，from 是目录路径
		static FilePath getRelativePathForAsset(const FilePath& to, const FilePath& from);

		//to 和 from 必须都是 directory
		static FilePath getRelativePath(const FilePath& to, const FilePath& from);

		static GuGuUtf8Str getBaseFolderName(const GuGuUtf8Str& folderPath);

		static GuGuUtf8Str getNonExtensionFileName(const GuGuUtf8Str& fileName);

		GuGuUtf8Str getStr() const;

		bool isDir() const;

		bool isFile() const;

		static void getRidDot(GuGuUtf8Str& filePath);
	private:
		GuGuUtf8Str m_filePath;//absolute or relative path
	};
}