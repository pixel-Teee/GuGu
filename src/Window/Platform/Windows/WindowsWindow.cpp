#include <pch.h>

#include "WindowsWindow.h"

namespace GuGu {
	std::shared_ptr<Window> CreateWindowFactory()
	{
		return std::make_shared<WindowsWindow>();
	}

	void WindowsWindow::ToGeneratePlatformWindow()
	{
		const wchar_t windowClassName[] = L"GuGuWindowClass";

		m_windowHandle = CreateWindowEx(
			0,//optional window style
			windowClassName,//window class
			L"GuGuWindow",//window title
			WS_OVERLAPPEDWINDOW,

			0, 0, 1280, 720,//size and position
			nullptr,//parent window
			nullptr,//menu
			m_ownerApplicationHandle,//owner application handle
			nullptr
		);

		RECT rcWindow;
		RECT rcClient;
		int32_t borderWidth, borderHeight;
		GetWindowRect(m_windowHandle, &rcWindow);
		GetClientRect(m_windowHandle, &rcClient);

		borderWidth = (rcWindow.right - rcWindow.left)
			- (rcClient.right - rcClient.left);
		borderHeight = (rcWindow.bottom - rcWindow.top)
			- (rcClient.bottom - rcClient.top);

		SetWindowPos(m_windowHandle, 0, 0, 0, borderWidth + 1280,
			borderHeight + 720, SWP_NOMOVE | SWP_NOZORDER);


		GuGu_LOGE(u8"create window error!");

		ShowWindow(m_windowHandle, m_startCmdShow);
	}

	void WindowsWindow::setNativeApplicationHandleAndCmdShowToCreateWindow(HINSTANCE applicationHandle, int32_t cmdShow)
	{
		m_ownerApplicationHandle = applicationHandle;
		m_startCmdShow = cmdShow;
	}

	HWND WindowsWindow::getNativeWindowHandle()
	{
		return m_windowHandle;
	}

}