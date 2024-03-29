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
			ADDKEYMAP(VK_BACK, "BackSpace"); //把windows平台相关的虚拟键映射到字符串
			ADDKEYMAP(VK_LEFT, "Left");
			ADDKEYMAP(VK_RIGHT, "Right");
		}
#undef ADDKEYMAP
		return numMappings;
	}
	uint32_t WindowsInput::getCharKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings)
	{
		return PlatformInput::getStandardPrintableKeyMap(keyCodes, keyNames, maxMappings, true, false);
	}
}