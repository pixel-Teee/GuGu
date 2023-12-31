#pragma once

#include <windows.h>
#include <Window/Window.h>

namespace GuGu {
	class WindowsWindow : public Window {
	public:
		virtual void ToGeneratePlatformWindow() override;

		void setNativeApplicationHandleAndCmdShowToCreateWindow(HINSTANCE applicationHandle, int32_t cmdShow);

		HWND getNativeWindowHandle();
	private:
		HWND m_windowHandle;
		HINSTANCE m_ownerApplicationHandle;
		int32_t m_startCmdShow;
	};
}