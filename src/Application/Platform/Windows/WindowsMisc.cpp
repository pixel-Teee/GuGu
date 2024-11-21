#include <pch.h>
#include "WindowsMisc.h"

#include <Core/GuGuUtf8Str.h>
#include <Core/UI/WindowWidget.h>
#include <Window/Platform/Windows/WindowsWindow.h>
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
	void WindowsMisc::getSaveFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName)
	{
		std::shared_ptr<WindowsWindow> platformWindow = std::static_pointer_cast<WindowsWindow>(ownerWindow->getNativeWindow());
		if (platformWindow)
		{
			OPENFILENAME ofn;//open file name
			TCHAR szFile[MAX_PATH] = TEXT("");
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = platformWindow->getNativeWindowHandle();
			ofn.lpstrFile = szFile;//用户选择的文件名存储在这里
			ofn.nMaxFile = sizeof(szFile);//最大文件路径长度
			ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");//文件过滤器
			ofn.nFilterIndex = 1;//默认选择第一个过滤器
			ofn.lpstrFileTitle = NULL;//文件标题指针，通常为NULL
			ofn.nMaxFileTitle = 0;//文件标题的最大长度，通常为0
			ofn.lpstrInitialDir = NULL;//初始目录，通常为NULL
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

			if (GetSaveFileName(&ofn))
			{
				//note:output file name
				fileName = GuGuUtf8Str::fromUtf16ToUtf8(ofn.lpstrFile);
				GuGu_LOGD("%s", fileName.getStr());
			}
			else
			{

				DWORD error = CommDlgExtendedError();
				if (error != 0) {
					switch (error) {
					case CDERR_DIALOGFAILURE:
						GuGu_LOGD("%s", "create dialog failure");
						break;
					case CDERR_FINDRESFAILURE:
						GuGu_LOGD("%s", "finders failure");
						break;
					case CDERR_INITIALIZATION:
						GuGu_LOGD("%s", "initialization");
						break;
						// 可以添加更多的错误处理代码
					}
				}
			}
		}	
	}
}
