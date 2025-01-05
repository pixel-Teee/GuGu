#pragma once

#include <Application/ApplicationMisc.h>

namespace GuGu {
	struct WindowsMisc : public ApplicationMisc
	{
		static void ClipboardCopy(const GuGuUtf8Str& str);
		static void ClipboardPaste(GuGuUtf8Str& str);

		static void getSaveOrOpenFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, const GuGuUtf8Str& initDir, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName, const std::vector<GuGuUtf8Str>& filterArray);
	};

	typedef WindowsMisc PlatformMisc;
}