#include <pch.h>

#include "FilePath.h"

namespace GuGu {
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
		return FilePath();
	}
}