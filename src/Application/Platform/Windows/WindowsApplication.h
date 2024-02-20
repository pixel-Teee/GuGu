#pragma once

#include <vector>

#include <windows.h>

#include <Application/Application.h>

namespace GuGu {
	class WindowsWindow;
	class WindowsApplication : public Application
	{
	public:
		WindowsApplication();

		virtual void init() override;

		virtual void pumpMessage() override;

		virtual std::shared_ptr<Window> getWindow(uint32_t index) override;

		std::vector<std::shared_ptr<WindowsWindow>> getPlatformWindows();

		static std::shared_ptr<WindowsApplication> getApplication();

		void setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow);
	private:
		HINSTANCE m_applicationInstance;

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;
	};
}