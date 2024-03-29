#include <pch.h>

#include "Input.h"

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	uint32_t GenericPlatformInput::getStandardPrintableKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings, bool bMapUppercaseKeys, bool bMapLowercaseKeys)
	{
		uint32_t numMappings = 0;
#define ADDKEYMAP(KeyCode, KeyName) if(numMappings < maxMappings) { keyCodes[numMappings] = KeyCode; keyNames[numMappings] = KeyName; ++numMappings; }

		//if (numMappings < maxMappings) {
		//	keyCodes[numMappings] = '0'; keyNames[numMappings] = "Zero"; ++numMappings;
		//};

		//keyCodes存放实际按键的字母，而keyNames则存放键的英文名
		ADDKEYMAP('0', "Zero");

#undef ADDKEYMAP

		return numMappings;
	}
}