#pragma once

#include <Application/Input.h>

namespace GuGu{
    class GuGuUtf8Str;
    class AndroidInput : public GenericPlatformInput
    {
    public:
        static uint32_t getKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings);
        static uint32_t getCharKeyMap(uint32_t* keyCodes, GuGuUtf8Str* keyNames, uint32_t maxMappings);
    };

    typedef AndroidInput PlatformInput;
}