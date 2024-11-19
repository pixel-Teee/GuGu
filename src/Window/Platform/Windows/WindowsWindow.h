#pragma once

#include <Window/Window.h>
#include <windows.h>

namespace GuGu {
	class WindowsWindow : public Window {
	public:
		WindowsWindow();

		virtual ~WindowsWindow();

		virtual void ToGeneratePlatformWindow(std::shared_ptr<WindowWidget> inWindowWidget) override;

		virtual float getDpiFactor() override;

		virtual math::float2 getWindowScreenSpacePosition() override;

		void setNativeApplicationHandleAndCmdShowToCreateWindow(HINSTANCE applicationHandle, int32_t cmdShow);

		HWND getNativeWindowHandle();

		virtual void moveWindowTo(math::float2 newPosition) override;

		virtual void reshapeWindow(math::float2 newPosition, math::float2 newSize) override;

		virtual void destroy() override;

		virtual void setWindowFocus() override;
	private:
		HWND m_windowHandle;
		HINSTANCE m_ownerApplicationHandle;
		int32_t m_startCmdShow;
		float m_dpiFactor;
	};
}