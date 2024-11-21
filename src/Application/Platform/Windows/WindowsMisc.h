#pragma once

#include <Application/ApplicationMisc.h>

namespace GuGu {
	struct WindowsMisc : public ApplicationMisc
	{
		static void ClipboardCopy(const GuGuUtf8Str& str);
		static void ClipboardPaste(GuGuUtf8Str& str);

		static void getSaveFilePathAndFileName(GuGuUtf8Str& filePath, GuGuUtf8Str& fileName);
	};

	typedef WindowsMisc PlatformMisc;
}