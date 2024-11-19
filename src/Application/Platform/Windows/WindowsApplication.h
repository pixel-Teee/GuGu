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

		virtual void init(std::shared_ptr<WindowWidget> inWindowWidget = nullptr) override;

		virtual void pumpMessage() override;

		virtual std::shared_ptr<Window> getWindow(uint32_t index) override;

		//virtual bool onWindowActivationChanged(const std::shared_ptr<Window>& window, const WindowActivation activationType);

		std::vector<std::shared_ptr<WindowsWindow>> getPlatformWindows();

		static std::shared_ptr<WindowsApplication> getApplication();

		void setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow);

		virtual ModifierKeysState getModifierKeys() const override;

		struct ModifierKey
		{
			enum Type
			{
				LeftShift, //VK_LSHIFT
				RightShift, //VK_RSHIFT
				LeftControl, //VK_LCONTROL
				RightControl, //VK_RCONTROL
				LeftAlt, //VK_LMENU
				RightAlt, //VK_RMENU
				CapsLock, //VK_CAPITAL
				Count,
			};
		};

		void setModifierKeyState(ModifierKey::Type key, bool value);

		virtual void makeWindow(std::shared_ptr<WindowWidget> windowWidget) override;

		virtual void showWindow(std::shared_ptr<WindowWidget> windowWidget) override;

		virtual void miniMizeWindow(std::shared_ptr<WindowWidget> windowWidget) override;
	private:
		HINSTANCE m_applicationInstance;

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;

		bool m_modifierKeyState[ModifierKey::Count];		
	};
}