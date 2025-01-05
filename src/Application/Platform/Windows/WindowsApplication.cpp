#include "pch.h"
#include "WindowsApplication.h"

#include <Window/Platform/Windows/WindowsWindow.h>

#if USE_DX12
		#include <Renderer/Platform/D3D12/D3D12Renderer.h>
#else
	#if USE_VK
		#include <Renderer/Platform/Vulkan/VulkanRenderer.h>
	#endif
#endif

#include "windowsx.h"

#include <Core/UI/WindowWidget.h>

namespace GuGu {
	std::shared_ptr<WindowsApplication> globalApplication;

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	WindowsApplication::WindowsApplication()
	{
		std::memset(m_modifierKeyState, 0, sizeof(m_modifierKeyState));
	}
	void WindowsApplication::init(std::shared_ptr<WindowWidget> inWindowWidget)
	{
		Application::init();
		//create renderer
#if USE_DX12
		m_renderer = std::make_shared<D3D12Renderer>();
		m_renderer->init();
#else
		m_renderer = std::make_shared<VulkanRenderer>();
		m_renderer->init(inWindowWidget);
#endif
	}
	void WindowsApplication::pumpMessage()
	{
		MSG msg = {};

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			setExit(true);
	}
	std::shared_ptr<Window> WindowsApplication::getWindow(uint32_t index)
	{
		//check index
		return m_windows[index];
	}
	std::vector<std::shared_ptr<WindowsWindow>> WindowsApplication::getPlatformWindows()
	{
		return m_windows;
	}
	std::shared_ptr<WindowsApplication> WindowsApplication::getApplication()
	{
		return globalApplication;
	}
	std::shared_ptr<Application> Application::getApplication()
	{
		return globalApplication;
	}
	void WindowsApplication::setNativeApplicationHandleAndCmdShow(HINSTANCE applicationInstance, int32_t cmdShow)
	{
		m_applicationInstance = applicationInstance;

		//register window class
		const wchar_t windowClassName[] = L"GuGuWindowClass";
		WNDCLASS wc = {};
		wc.style = CS_DBLCLKS;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = applicationInstance;
		wc.lpszClassName = windowClassName;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClass(&wc);
	}

	ModifierKeysState WindowsApplication::getModifierKeys() const
	{
		return ModifierKeysState(m_modifierKeyState[ModifierKey::LeftControl], m_modifierKeyState[ModifierKey::LeftShift]);
	}

	void WindowsApplication::setModifierKeyState(ModifierKey::Type key, bool value)
	{
		m_modifierKeyState[key] = value;
	}

	void WindowsApplication::makeWindow(std::shared_ptr<WindowWidget> windowWidget)
	{
		std::shared_ptr<WindowsWindow> window = std::make_shared<WindowsWindow>();
		//todo:这里 cmdShow 需要修复
		window->setNativeApplicationHandleAndCmdShowToCreateWindow(m_applicationInstance, true);
		m_windows.push_back(window);
		windowWidget->assocateWithNativeWindow(window);//关联
		m_windowWidgets.push_back(windowWidget);
		window->ToGeneratePlatformWindow(windowWidget);
	}

	void WindowsApplication::showWindow(std::shared_ptr<WindowWidget> windowWidget)
	{
		//获取 UIRenderPass 
		std::shared_ptr<Renderer> renderer = getRenderer();
		//UIRenderPass* uiRenderPass = renderer->getUIRenderPass();
		//uiRenderPass->showWindow(windowWidget);
		//create swap chain and surface
		renderer->createSurface(windowWidget);
		renderer->createSwapChain(windowWidget);

		//set window focus
		if (windowWidget->supportsKeyboardFocus() && windowWidget->isFocusedInitially())
		{
			windowWidget->getNativeWindow()->setWindowFocus();
		}
		windowWidget->getNativeWindow()->show();
	}

	void WindowsApplication::miniMizeWindow(std::shared_ptr<WindowWidget> windowWidget)
	{
		std::shared_ptr<WindowsWindow> windowsWindow = std::static_pointer_cast<WindowsWindow>(windowWidget->getNativeWindow());
		ShowWindow(windowsWindow->getNativeWindowHandle(), SW_MINIMIZE);
	}

	math::box2 WindowsApplication::getWorkArea(const math::box2& currentWindow) const
	{
		RECT windowsWindowDim;
		windowsWindowDim.left = currentWindow.m_mins.x;
		windowsWindowDim.top = currentWindow.m_mins.y;
		windowsWindowDim.right = currentWindow.m_maxs.x;
		windowsWindowDim.bottom = currentWindow.m_maxs.y;

		HMONITOR hBestMonitor = MonitorFromRect(&windowsWindowDim, MONITOR_DEFAULTTONEAREST);

		MONITORINFO MonitorInfo;
		MonitorInfo.cbSize = sizeof(MonitorInfo);
		GetMonitorInfo(hBestMonitor, &MonitorInfo);

		math::box2 workArea;
		workArea.m_mins.x = MonitorInfo.rcWork.left;
		workArea.m_mins.y = MonitorInfo.rcWork.top;
		workArea.m_maxs.x = MonitorInfo.rcWork.right;
		workArea.m_maxs.x = MonitorInfo.rcWork.bottom;

		return workArea;
	}

	WindowZone::Type WindowsApplication::getWindowZoneForPoint(const std::shared_ptr<Window>& window, const int32_t X, const int32_t Y)
	{
		std::shared_ptr<WindowWidget> windowWidget;
		for (size_t i = 0; i < m_windowWidgets.size(); ++i)
		{
			if (m_windowWidgets[i]->getNativeWindow() == window)
			{
				windowWidget = m_windowWidgets[i];
				break;
			}
		}

		if (windowWidget)
		{
			return windowWidget->getCurrentWindowZone(math::float2(X, Y));
		}

		return WindowZone::NontInWindow;
	}

	math::float2 WindowsApplication::getCursorPos() const
	{
#ifdef WIN32
		POINT CursorPos;
		::GetCursorPos(&CursorPos);

		return math::float2(CursorPos.x, CursorPos.y);
#endif
	}

	static bool FolderExists(const GuGuUtf8Str& folderPath)
	{
		DWORD dwAttrib = GetFileAttributesA(folderPath.getStr());
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	GuGuUtf8Str Application::GetExecutableFilePath()
	{
		GuGuUtf8Str executableFilePath;
#if WIN32
		char path[260] = { 0 };
		if (GetModuleFileNameA(nullptr, path, sizeof(path)) == 0)
			return "";

		GuGuUtf8Str tmpPath = path;
		GuGuUtf8Str parentPath;
		for (size_t i = 0; i < tmpPath.len(); ++i)
		{
			if (tmpPath[i] == "\\")
			{
				parentPath += "/";
				//i += 2;
				continue;
			}
			parentPath += tmpPath[i];
		}
		executableFilePath = parentPath.findLastOf("/") != -1 ? parentPath.substr(0, parentPath.findLastOf("/")) : "";
		return executableFilePath;
#else
#ifdef ANDROID
		return GuGuUtf8Str("");
#endif
#endif
	}

	GuGuUtf8Str Application::GetContentDirectoryWithExecutable()
	{
		//first:to find content folder, otherwise use executable folder

		char path[260] = { 0 };
		if (GetModuleFileNameA(nullptr, path, sizeof(path)) == 0)
			return "";

		GuGuUtf8Str tmpPath = path;
		GuGuUtf8Str parentPath;
		for (size_t i = 0; i < tmpPath.len(); ++i)
		{
			if (tmpPath[i] == "\\")
			{
				parentPath += "/";
				//i += 2;
				continue;
			}
			parentPath += tmpPath[i];
		}
#if WIN32
		GuGuUtf8Str executablePath = parentPath.findLastOf("/") != -1 ? parentPath.substr(0, parentPath.findLastOf("/")) : "";
		GuGuUtf8Str contentPath = executablePath + "/../../../content";
		if (FolderExists(contentPath))
			return contentPath;
		else
		{
			//if (parentPath.findLastOf("/") != -1)
			//	return parentPath.substr(0, parentPath.findLastOf("/"));
			//return "";
			//create content path
			::CreateDirectory(contentPath.getUtf16String().c_str(), NULL);
			return contentPath;
		}
#else
#ifdef ANDROID
		if (parentPath.findLastOf("/") != -1)
			return parentPath.substr(0, parentPath.findLastOf("/"));
		return "";
#endif
#endif
	}

	GuGuUtf8Str Application::GetDirectoryWithExecutable()
	{
		//first:to find content folder, otherwise use executable folder

		char path[260] = { 0 };
		if (GetModuleFileNameA(nullptr, path, sizeof(path)) == 0)
			return "";

		GuGuUtf8Str tmpPath = path;
		GuGuUtf8Str parentPath;
		for (size_t i = 0; i < tmpPath.len(); ++i)
		{
			if (tmpPath[i] == "\\")
			{
				parentPath += "/";
				//i += 2;
				continue;
			}
			parentPath += tmpPath[i];
		}
#if WIN32
		GuGuUtf8Str executablePath = parentPath.findLastOf("/") != -1 ? parentPath.substr(0, parentPath.findLastOf("/")) : "";
		GuGuUtf8Str contentPath = executablePath + "/../../../asset";
		if (FolderExists(contentPath))
			return contentPath;
		else
		{
			if (parentPath.findLastOf("/") != -1)
				return parentPath.substr(0, parentPath.findLastOf("/"));
			return "";
		}
#else
	#ifdef ANDROID
		if (parentPath.findLastOf("/") != -1)
			return parentPath.substr(0, parentPath.findLastOf("/"));
		return "";
	#endif
#endif
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
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MOUSEMOVE:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			{
				POINT cursorPoint;
				cursorPoint.x = GET_X_LPARAM(lParam);
				cursorPoint.y = GET_Y_LPARAM(lParam);
				ClientToScreen(hwnd, &cursorPoint);

				std::shared_ptr<Window> window;
				//find native window
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					if (windows[i]->getNativeWindowHandle() == hwnd)
					{
						window = windows[i];
					}
				}
				bool bDoubleClick = false;
				//GuGu_LOGD("%d %d", cursorPoint.x, cursorPoint.y);
				switch (uMsg)
				{
					case WM_LBUTTONDBLCLK:
					{
						globalApplication->onMouseDoubleClick(window, MouseButtons::Type::Left, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_LBUTTONDOWN:
					{
						globalApplication->onMouseDown(window, MouseButtons::Type::Left, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_LBUTTONUP:
					{
						globalApplication->onMouseUp(window, MouseButtons::Type::Left, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_MOUSEMOVE:
					{
						globalApplication->onMouseMove(window, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_RBUTTONDBLCLK:
					{
						globalApplication->onMouseDoubleClick(window, MouseButtons::Type::Right, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_RBUTTONDOWN:
					{
						globalApplication->onMouseDown(window, MouseButtons::Type::Right, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_RBUTTONUP:
					{
						globalApplication->onMouseUp(window, MouseButtons::Type::Right, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
				}
				break;
			}
			case WM_KEYDOWN:
			{
				const int32_t win32Key = wParam;//获取虚拟码

				int32_t actualKey = win32Key;

				switch (win32Key)
				{
					case VK_SHIFT:
					{
						//分清楚left和right shift
						actualKey = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
						if (actualKey == VK_LSHIFT)
						{
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::LeftShift, true);
						}
						else
						{
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::RightShift, true);
						}
						break;
					}
					case VK_CONTROL:
					{
						if ((lParam & 0x1000000) == 0)
						{
							actualKey = VK_LCONTROL;
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::LeftControl, true);
						}
						else
						{
							actualKey = VK_RCONTROL;
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::RightControl, true);
						}
						break;
					}
				}

				//获取字母码点从虚拟键的按压，将虚拟码转换为字符值
				uint32_t charCode = MapVirtualKey(win32Key, MAPVK_VK_TO_CHAR);

				globalApplication->onKeyDown(actualKey, charCode);
				break;
			}
			case WM_KEYUP:
			{
				const int32_t win32Key = wParam;//获取虚拟码

				int32_t actualKey = win32Key;

				switch (win32Key)
				{
					case VK_SHIFT:
					{
						//分清楚left和right shift
						actualKey = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
						if (actualKey == VK_LSHIFT)
						{
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::LeftShift, false);
						}
						else
						{
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::RightShift, false);
						}
						break;
					}
					case VK_CONTROL:
					{
						if ((lParam & 0x1000000) == 0)
						{
							actualKey = VK_LCONTROL;
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::LeftControl, false);
						}
						else
						{
							actualKey = VK_RCONTROL;
							globalApplication->setModifierKeyState(WindowsApplication::ModifierKey::Type::RightControl, false);
						}
						break;
					}
				}

				//获取字母码点从虚拟键的按压，将虚拟码转换为字符值
				uint32_t charCode = MapVirtualKey(win32Key, MAPVK_VK_TO_CHAR);

				//添加onKeyUp
				//globalApplication->onKeyDown(actualKey, charCode);
				break;
			}
			case WM_CHAR:
			{
				uint32_t utf32CodePoint = wParam;

				std::string buffer;
				if (utf32CodePoint < 0x80)
					buffer.push_back(utf32CodePoint);
				else if (utf32CodePoint < 0x800)
				{
					buffer.push_back((utf32CodePoint >> 6) | 0xC0);
					buffer.push_back((utf32CodePoint & 0x3F) | 0xC0);
				}
				else if (utf32CodePoint < 0x10000)
				{
					buffer.push_back((utf32CodePoint >> 12) | 0xE0);
					buffer.push_back(((utf32CodePoint >> 6) & 0x3F) | 0x80);
					buffer.push_back((utf32CodePoint & 0x3F) | 0x80);
				}
				else if (utf32CodePoint < 0x110000)
				{
					buffer.push_back((utf32CodePoint >> 18) | 0xF0);
					buffer.push_back(((utf32CodePoint >> 12) & 0x3F) | 0x80);
					buffer.push_back(((utf32CodePoint >> 6) & 0x3F) | 0x80);
					buffer.push_back((utf32CodePoint & 0x3F) | 0x80);
				}

				GuGuUtf8Str Character(buffer);

				//from utf32 to utf8
				globalApplication->onKeyChar(Character);
				break;
			}
			case WM_SIZE:
			{
				//on size changed
				std::shared_ptr<Window> window;
				//find native window
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					if (windows[i]->getNativeWindowHandle() == hwnd)
					{
						window = windows[i];
					}
				}
				globalApplication->OnResizeChanged(window);
				break;
			}
			case WM_DESTROY:
			{
				//PostQuitMessage(0);
				break;
			}
			case WM_CLOSE:
			{
				//std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				//for (int32_t i = 0; i < windows.size(); ++i)
				//{
				//	DestroyWindow(windows[i]->getNativeWindowHandle());
				//}
				//globalApplication->setExit(true);
				break;
			}
			case WM_ACTIVATE:
			{
				WindowActivation activationType;
				if (LOWORD(wParam) & WA_ACTIVE)
				{
					activationType = WindowActivation::Activate;
					GuGu_LOGD("%d", activationType);
				}
				else if (LOWORD(wParam) & WA_CLICKACTIVE)
				{
					activationType = WindowActivation::Activate; //todo:fix active by mouse
				}
				else
				{
					activationType = WindowActivation::Deactivate;
				}
				//todo:add update all modifier key states
				std::shared_ptr<Window> window;
				//find native window
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					if (windows[i]->getNativeWindowHandle() == hwnd)
					{
						window = windows[i];
					}
				}
				if (window)
				{
					bool result = false;
					result = globalApplication->onWindowActivationChanged(window, activationType);
				}
				break;
			}
			case WM_NCHITTEST:
			{
				RECT rcWindow;
				GetWindowRect(hwnd, &rcWindow);

				const int32_t localMouseX = (int32_t)(short)(LOWORD(lParam)) - rcWindow.left;
				const int32_t localMouseY = (int32_t)(short)(HIWORD(lParam)) - rcWindow.top;

				//todo:判断这个是不是普通窗口
				WindowZone::Type zone;
				std::shared_ptr<Window> window;
				//find native window
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					if (windows[i]->getNativeWindowHandle() == hwnd)
					{
						window = windows[i];
					}
				}
				std::shared_ptr<WindowWidget> windowWidget;
				int32_t windowWidgetsNumber = globalApplication->getWidowWidgets().size();
				for (int32_t i = 0; i < windowWidgetsNumber; ++i)
				{
					if (globalApplication->getWidowWidgets()[i]->getNativeWindow() == window)
					{
						windowWidget = globalApplication->getWidowWidgets()[i];
						break;
					}
				}
				static const LRESULT results[] = { HTNOWHERE, HTTOPLEFT, HTTOP, HTTOPRIGHT, HTLEFT, HTCLIENT,
					HTRIGHT, HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT,
					HTCAPTION, HTMINBUTTON, HTMAXBUTTON, HTCLOSE, HTSYSMENU };
				if (windowWidget->isRegularWindow())
				{
					zone = globalApplication->getWindowZoneForPoint(window, localMouseX, localMouseY);
					return results[zone];
				}
				return results[WindowZone::ClientArea];
				break;
			}
			case WM_MOVE:
			{
				const int32_t NewX = (int)(short)(LOWORD(lParam));
				const int32_t NewY = (int)(short)(HIWORD(lParam));
				
				std::shared_ptr<Window> window;
				//find native window
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					if (windows[i]->getNativeWindowHandle() == hwnd)
					{
						window = windows[i];
					}
				}
				globalApplication->onMovedWindow(window, NewX, NewY);
				//return 0;
				break;
			}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
