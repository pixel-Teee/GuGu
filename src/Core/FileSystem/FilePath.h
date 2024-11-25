#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class FilePath
	{
	public:
		FilePath();

		FilePath(const GuGuUtf8Str& filePath);

		FilePath(GuGuUtf8Str&& filePath);
		static FilePath getRelativePath(const FilePath& to, const FilePath& from);
	private:
		GuGuUtf8Str m_filePath;//absolute or relative path
	};
}