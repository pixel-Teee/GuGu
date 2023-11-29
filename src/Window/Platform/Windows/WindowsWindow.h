#pragma once

#include <Window/Window.h>

namespace GuGu {
	class WindowsWindow : public Window {
	public:
		virtual void DeferGeneratePlatformWindow() override;
	private:
		HWND mWindowHandle;
	};
}