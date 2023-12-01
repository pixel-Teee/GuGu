#pragma once

#include <Application/Application.h>

#include <windows.h>

#include <vector>

namespace GuGu {
	class WindowsWindow;
	class WindowsApplication : public Application
	{
	public:
		WindowsApplication();

		void setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow);

		virtual void pumpMessage() override;

		std::vector<std::shared_ptr<WindowsWindow>> getPlatformWindows();
	private:
		HINSTANCE m_applicationInstance;

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;

	};
}