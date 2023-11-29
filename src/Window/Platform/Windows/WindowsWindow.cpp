#include <pch.h>

#include "WindowsWindow.h"

#include <Windows.h>//windows platform

namespace GuGu {
	std::shared_ptr<Window> CreateWindowFactory()
	{
		return std::make_shared<WindowsWindow>();
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void WindowsWindow::DeferGeneratePlatformWindow()
	{
		const wchar_t ClassName[] = L"WindowsWindow";

		WNDCLASS wc = {};

		wc.lpfnWndProc = WindowProc;

		//need hInstance
		//wc.lpszClassName = ClassName;
	}

	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}