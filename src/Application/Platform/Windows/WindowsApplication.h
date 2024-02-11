#pragma once

#include <Application/Application.h>

#include <windows.h>

#include <vector>

namespace GuGu {
	class WindowsWindow;
	class D3D12Renderer;
	class WindowsApplication : public Application
	{
	public:
		WindowsApplication();

		void setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow);

		virtual void pumpMessage() override;

		virtual void init() override;

		std::vector<std::shared_ptr<WindowsWindow>> getPlatformWindows();

		static std::shared_ptr<WindowsApplication> getApplication();

		virtual std::shared_ptr<Window> getWindow(uint32_t index) override;
	private:
		HINSTANCE m_applicationInstance;

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;

		//std::shared_ptr<D3D12Renderer> m_renderer;
	};
}