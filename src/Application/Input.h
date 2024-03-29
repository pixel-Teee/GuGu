#pragma once

namespace GuGu{
	class GuGuUtf8Str;
	struct GenericPlatformInput
	{
	public:


	protected:
		//bMapUppercaseKeys会映射A,B,C到Keys::A, Keys::B, Keys::C
		//bMapLowercaseKeys会映射a,b,c到Keys::A, Keys::B, Keys::C
		static uint32_t getStandardPrintableKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings, bool bMapUppercaseKeys, bool bMapLowercaseKeys);
	};
}