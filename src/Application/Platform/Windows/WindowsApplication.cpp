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

namespace GuGu {
	std::shared_ptr<WindowsApplication> globalApplication;

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	WindowsApplication::WindowsApplication()
	{
		std::memset(m_modifierKeyState, 0, sizeof(m_modifierKeyState));
	}
	void WindowsApplication::init()
	{
		//create renderer
#if USE_DX12
		m_renderer = std::make_shared<D3D12Renderer>();
		m_renderer->init();
#else
		m_renderer = std::make_shared<VulkanRenderer>();
		m_renderer->init();
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
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = applicationInstance;
		wc.lpszClassName = windowClassName;
		//wc.hCursor = LoadCursor(applicationInstance, IDC_ARROW);
		RegisterClass(&wc);

		//test:create a window for test
		std::shared_ptr<WindowsWindow> window = std::make_shared<WindowsWindow>();
		window->setNativeApplicationHandleAndCmdShowToCreateWindow(m_applicationInstance, cmdShow);
		window->ToGeneratePlatformWindow();
		m_windows.push_back(window);
	}

	ModifierKeysState WindowsApplication::getModifierKeys() const
	{
		return ModifierKeysState(m_modifierKeyState[ModifierKey::LeftControl], m_modifierKeyState[ModifierKey::LeftShift]);
	}

	void WindowsApplication::setModifierKeyState(ModifierKey::Type key, bool value)
	{
		m_modifierKeyState[key] = value;
	}

	static bool FolderExists(const GuGuUtf8Str& folderPath)
	{
		DWORD dwAttrib = GetFileAttributesA(folderPath.getStr());
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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
				//GuGu_LOGD("%d %d", cursorPoint.x, cursorPoint.y);
				switch (uMsg)
				{
					case WM_LBUTTONDOWN:
					{
						globalApplication->onMouseDown(window, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_LBUTTONUP:
					{
						globalApplication->onMouseUp(window, math::float2(cursorPoint.x, cursorPoint.y));
						break;
					}
					case WM_MOUSEMOVE:
					{
						globalApplication->onMouseMove(window, math::float2(cursorPoint.x, cursorPoint.y));
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
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
			case WM_CLOSE:
			{
				std::vector<std::shared_ptr<WindowsWindow>> windows = globalApplication->getPlatformWindows();
				for (int32_t i = 0; i < windows.size(); ++i)
				{
					DestroyWindow(windows[i]->getNativeWindowHandle());
				}
				globalApplication->setExit(true);
				break;
			}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
