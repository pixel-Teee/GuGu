#include <pch.h>

#include "Input.h"

#include <Core/GuGuUtf8Str.h>
#include "Input.h"

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
		ADDKEYMAP('1', "One");
		ADDKEYMAP('2', "Two");
		ADDKEYMAP('3', "Three");
		ADDKEYMAP('4', "Four");
		ADDKEYMAP('5', "Five");
		ADDKEYMAP('6', "Six");
		ADDKEYMAP('7', "Seven");
		ADDKEYMAP('8', "Eight");
		ADDKEYMAP('9', "Nine");

		if (bMapUppercaseKeys)
		{
			ADDKEYMAP('A', "A");
			ADDKEYMAP('B', "B");
			ADDKEYMAP('C', "C");
			ADDKEYMAP('D', "D");
			ADDKEYMAP('E', "E");
			ADDKEYMAP('F', "F");
			ADDKEYMAP('G', "G");
			ADDKEYMAP('H', "H");
			ADDKEYMAP('I', "I");
			ADDKEYMAP('J', "J");
			ADDKEYMAP('K', "K");
			ADDKEYMAP('L', "L");
			ADDKEYMAP('M', "M");
			ADDKEYMAP('N', "N");
			ADDKEYMAP('O', "O");
			ADDKEYMAP('P', "P");
			ADDKEYMAP('Q', "Q");
			ADDKEYMAP('R', "R");
			ADDKEYMAP('S', "S");
			ADDKEYMAP('T', "T");
			ADDKEYMAP('U', "U");
			ADDKEYMAP('V', "V");
			ADDKEYMAP('W', "W");
			ADDKEYMAP('X', "X");
			ADDKEYMAP('Y', "Y");
			ADDKEYMAP('Z', "Z");
		}

		if (bMapLowercaseKeys)
		{
			ADDKEYMAP('a', "A");
			ADDKEYMAP('b', "B");
			ADDKEYMAP('c', "C");
			ADDKEYMAP('d', "D");
			ADDKEYMAP('e', "E");
			ADDKEYMAP('f', "F");
			ADDKEYMAP('g', "G");
			ADDKEYMAP('h', "H");
			ADDKEYMAP('i', "I");
			ADDKEYMAP('j', "J");
			ADDKEYMAP('k', "K");
			ADDKEYMAP('l', "L");
			ADDKEYMAP('m', "M");
			ADDKEYMAP('n', "N");
			ADDKEYMAP('o', "O");
			ADDKEYMAP('p', "P");
			ADDKEYMAP('q', "Q");
			ADDKEYMAP('r', "R");
			ADDKEYMAP('s', "S");
			ADDKEYMAP('t', "T");
			ADDKEYMAP('u', "U");
			ADDKEYMAP('v', "V");
			ADDKEYMAP('w', "W");
			ADDKEYMAP('x', "X");
			ADDKEYMAP('y', "Y");
			ADDKEYMAP('z', "Z");
		}
		
#undef ADDKEYMAP

		return numMappings;
	}

}