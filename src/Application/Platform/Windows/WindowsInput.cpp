#include <pch.h>

#include "WindowsInput.h"

#include <windows.h>

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	uint32_t WindowsInput::getKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings)
	{
#define ADDKEYMAP(KeyCode, KeyName) if(numMappings < maxMappings) { keyCodes[numMappings] = KeyCode; keyNames[numMappings] = KeyName; ++numMappings;}
		uint32_t numMappings = 0;

		if (keyCodes && keyNames && (maxMappings > 0))
		{
			ADDKEYMAP(VK_LBUTTON, "LeftMouseButton");
			ADDKEYMAP(VK_RBUTTON, "RightMouseButton");
			ADDKEYMAP(VK_MBUTTON, "MiddleMouseButton");
			ADDKEYMAP(VK_XBUTTON1, "ThumbMouseButton");
			ADDKEYMAP(VK_XBUTTON2, "ThumbMouseButton2");

			ADDKEYMAP(VK_BACK, "BackSpace"); //把windows平台相关的虚拟键映射到字符串
			ADDKEYMAP(VK_TAB, "Tab");
			ADDKEYMAP(VK_RETURN, "Enter");
			ADDKEYMAP(VK_PAUSE, "Pause");

			ADDKEYMAP(VK_LEFT, "Left");
			ADDKEYMAP(VK_RIGHT, "Right");
			ADDKEYMAP(VK_UP, "UP");
			ADDKEYMAP(VK_DOWN, "DOWN");

			ADDKEYMAP(VK_LSHIFT, "LeftShift");
			ADDKEYMAP(VK_RSHIFT, "RightShift");
			ADDKEYMAP(VK_LCONTROL, "LeftControl");
			ADDKEYMAP(VK_RCONTROL, "RightControl");
			ADDKEYMAP(VK_LMENU, "LeftAlt");
			ADDKEYMAP(VK_RMENU, "RightAlt");
			ADDKEYMAP(VK_LWIN, "LeftCommand");
			ADDKEYMAP(VK_RWIN, "RightCommand");
		}
#undef ADDKEYMAP
		return numMappings;
	}
	uint32_t WindowsInput::getCharKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings)
	{
		return PlatformInput::getStandardPrintableKeyMap(keyCodes, keyNames, maxMappings, true, false);
	}
}