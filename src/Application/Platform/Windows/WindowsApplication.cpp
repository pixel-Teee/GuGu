#include "pch.h"
#include "WindowsApplication.h"
#include <Window/Platform/Windows/WindowsWindow.h>

namespace GuGu {
	std::shared_ptr<WindowsApplication> globalApplication;
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	WindowsApplication::WindowsApplication()
	{
	
	}
	void WindowsApplication::setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow)
	{
		m_applicationInstance = applicationInstance;

		//register window class
		const wchar_t windowClassName[] = L"GuGuWindowClass";
		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = applicationInstance;
		wc.lpszClassName = windowClassName;
		RegisterClass(&wc);
		
		//test:create a window for test
		std::shared_ptr<WindowsWindow> window = std::make_shared<WindowsWindow>();
		window->setNativeApplicationHandleAndCmdShowToCreateWindow(m_applicationInstance, cmdShow);
		window->ToGeneratePlatformWindow();
		m_windows.push_back(window);
	}

	void WindowsApplication::pumpMessage()
	{
		MSG msg = {};
		while (GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			TranslateMessage(&msg);
			TranslateMessage(&msg);
		}
	}

	std::vector<std::shared_ptr<WindowsWindow>> WindowsApplication::getPlatformWindows()
	{
		return m_windows;
	}

	std::shared_ptr<Application> CreateApplicationFactory()
	{
		globalApplication = std::make_shared<WindowsApplication>();
		return globalApplication;
	}
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
		case WM_CLOSE:
			std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
			for (int32_t i = 0; i < windows.size(); ++i)
			{
				DestroyWindow(windows[i]->getNativeWindowHandle());
			}
			globalApplication->setExit(true);
			break;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
