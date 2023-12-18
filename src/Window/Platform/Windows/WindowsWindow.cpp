#include <pch.h>

#include "WindowsWindow.h"

#include <Windows.h>//windows platform

namespace GuGu {
	std::shared_ptr<Window> CreateWindowFactory()
	{
		return std::make_shared<WindowsWindow>();
	}

	void WindowsWindow::ToGeneratePlatformWindow()
	{
		const wchar_t windowClassName[] = L"GuGuWindowClass";

		m_windowHandle = CreateWindowEx(
			0, //optional window style
			windowClassName, //window class
			L"GuGuWindow", //window title
			WS_OVERLAPPEDWINDOW,

			0, 0, 800, 600, //size and position
			nullptr,//parent window
			nullptr,//menu
			m_ownerApplicationHandle, //owner application handle
			nullptr
		);

		if (m_windowHandle == nullptr) return; //todo:report create window error

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