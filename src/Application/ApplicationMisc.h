#pragma once

#include <vector>

namespace GuGu {
	class GuGuUtf8Str;
	class WindowWidget;
	struct ApplicationMisc
	{
		static void ClipboardCopy(const GuGuUtf8Str& str);
		static void ClipboardPaste(GuGuUtf8Str& str);

		//save file
		static void getSaveOrOpenFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, const GuGuUtf8Str& initDir, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName, const std::vector<GuGuUtf8Str>& filterArray);
	};
}