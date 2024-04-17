#include <pch.h>

#include "WindowsWindow.h"

namespace GuGu {
	std::shared_ptr<Window> CreateWindowFactory()
	{
		return std::make_shared<WindowsWindow>();
	}

	WindowsWindow::WindowsWindow()
		: m_dpiFactor(1.0f)
	{
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	void WindowsWindow::ToGeneratePlatformWindow()
	{
		const wchar_t windowClassName[] = L"GuGuWindowClass";

		int32_t screentWidth = GetSystemMetrics(SM_CXSCREEN);
		int32_t screentHeight = GetSystemMetrics(SM_CYSCREEN);

		int32_t x = (screentWidth - 1280) / 2;
		int32_t y = (screentHeight - 720) / 2;

		m_windowHandle = CreateWindowEx(
			0,//optional window style
			windowClassName,//window class
			L"GuGuWindow",//window title
			WS_POPUP,

			x, y, 1280, 720,//position and size
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

		//GuGu_LOGE(u8"create window error!");

		ShowWindow(m_windowHandle, m_startCmdShow);

		//------get dpi scale------
		HDC Context = GetDC(nullptr);
		int32_t dpi = GetDeviceCaps(Context, LOGPIXELSX);
		m_dpiFactor = (float)dpi / 96.0f;
		ReleaseDC(nullptr, Context);
	}

	float WindowsWindow::getDpiFactor()
	{
		return m_dpiFactor;
	}

	math::float2 WindowsWindow::getWindowScreenSpacePosition()
	{
		RECT windowRect;
		GetWindowRect(m_windowHandle, &windowRect);

		//RECT clientRect;
		//GetClientRect(m_windowHandle, &clientRect);
		//int32_t titleBarHeight = (windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top);

		//int32_t titleBarHeight = GetSystemMetrics(SM_CYCAPTION); //todo:fix this
		return math::float2(windowRect.left, windowRect.top);
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