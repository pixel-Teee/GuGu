#include <pch.h>

#include "WindowsWindow.h"

#include <Core/UI/WindowWidget.h>

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

	void WindowsWindow::ToGeneratePlatformWindow(std::shared_ptr<WindowWidget> inWindowWidget)
	{
		const wchar_t windowClassName[] = L"GuGuWindowClass";

		int32_t screentWidth = GetSystemMetrics(SM_CXSCREEN);
		int32_t screentHeight = GetSystemMetrics(SM_CYSCREEN);

		math::int2 windowWidthAndHeight = inWindowWidget->getViewportSize();
		math::float2 windowPosition = inWindowWidget->getPositionInScreen();

		//int32_t x = (screentWidth - windowWidthAndHeight.x) / 2;
		//int32_t y = (screentHeight - windowWidthAndHeight.y) / 2;
		int32_t x = windowPosition.x;
		int32_t y = windowPosition.y;

		std::shared_ptr<WindowWidget> parentWindow = inWindowWidget->getParentWindow().lock();

		HWND parentWindowHandle = parentWindow ? std::static_pointer_cast<WindowsWindow>(parentWindow->getNativeWindow())->m_windowHandle : NULL;

		m_windowHandle = CreateWindowEx(
			0,//optional window style
			windowClassName,//window class
			L"GuGuWindow",//window title
			WS_POPUP,
			x, y, windowWidthAndHeight.x, windowWidthAndHeight.y,//position and size
			parentWindowHandle,//parent window
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

		//show();

		SetWindowPos(m_windowHandle, 0, 0, 0, borderWidth + windowWidthAndHeight.x,
			borderHeight + windowWidthAndHeight.y, SWP_NOMOVE | SWP_NOZORDER);

		//GuGu_LOGE(u8"create window error!");

		//ShowWindow(m_windowHandle, m_startCmdShow);

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

	void WindowsWindow::moveWindowTo(math::float2 newPosition)
	{
		SetWindowPos(m_windowHandle, nullptr, newPosition.x, newPosition.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}

	void WindowsWindow::reshapeWindow(math::float2 newPosition, math::float2 newSize)
	{
		//调整windows window的窗口大小
		::SetWindowPos(m_windowHandle, nullptr, newPosition.x, newPosition.y, newSize.x, newSize.y, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	void WindowsWindow::destroy()
	{
		::DestroyWindow(m_windowHandle);
	}

	void WindowsWindow::setWindowFocus()
	{
		if (::GetFocus() != m_windowHandle)
		{
			::SetFocus(m_windowHandle);
		}
	}
	void WindowsWindow::show()
	{
		::ShowWindow(m_windowHandle, SW_SHOW);
	}
}