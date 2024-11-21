#pragma once

namespace GuGu {
	class GuGuUtf8Str;
	struct ApplicationMisc
	{
		static void ClipboardCopy(const GuGuUtf8Str& str);
		static void ClipboardPaste(GuGuUtf8Str& str);

		//save file
		static void getSaveFilePathAndFileName(GuGuUtf8Str& filePath, GuGuUtf8Str& fileName);
	};
}