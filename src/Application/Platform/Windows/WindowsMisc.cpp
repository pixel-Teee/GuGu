#include <pch.h>
#include "WindowsMisc.h"

#include <Core/GuGuUtf8Str.h>

#include <Windows.h>

namespace GuGu {
	//剪切板功能
	void WindowsMisc::ClipboardCopy(const GuGuUtf8Str& str)
	{
		if (OpenClipboard(GetActiveWindow()))
		{
			EmptyClipboard();

			std::wstring utf16Str = str.getUtf16String();

			size_t allocSize = sizeof(wchar_t) * (utf16Str.length() + 1);
			HGLOBAL globalMem = GlobalAlloc(GMEM_MOVEABLE, allocSize);
			
			wchar_t* data = (wchar_t*)GlobalLock(globalMem);
			std::memcpy(data, utf16Str.c_str(), sizeof(wchar_t) * (utf16Str.length() + 1));

			GlobalUnlock(globalMem);
			if (SetClipboardData(CF_UNICODETEXT, globalMem) == NULL)
				GuGu_LOGD("SetClipboard failed with error code %i", (uint32_t)GetLastError());

			CloseClipboard();
		}
	}
	void WindowsMisc::ClipboardPaste(GuGuUtf8Str& str)
	{
		//粘贴文本到str
		if (OpenClipboard(GetActiveWindow()))
		{
			HGLOBAL globaleMem = NULL;
			bool unicode = 0;
			globaleMem = GetClipboardData(CF_UNICODETEXT);
			unicode = true;
			if (!globaleMem)
			{
				globaleMem = GetClipboardData(CF_TEXT);
				unicode = false;
			}
			if (!globaleMem)
			{
				str = "";
			}
			else
			{
				wchar_t* data = (wchar_t*)GlobalLock(globaleMem);
				//wchar_t* testData = (wchar_t*)data;
				if (unicode)
				{
					//从utf16转换为utf8
					str = GuGuUtf8Str::fromUtf16ToUtf8(data);
				}
				else
				{
					str = (char*)data;//直接从ansi char 转换为 u8
				}
				GlobalUnlock(globaleMem);
			}
			CloseClipboard();
		}
	}
	void WindowsMisc::getSaveFilePathAndFileName(GuGuUtf8Str& filePath, GuGuUtf8Str& fileName)
	{
	}
}
