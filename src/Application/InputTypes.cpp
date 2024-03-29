#include <pch.h>

#include "InputTypes.h"

#ifdef WIN32
#include <Platform/Windows/WindowsInput.h>
#else
#ifdef ANDROID
#include <Platform/Android/AndroidInput.h>
#endif
#endif

namespace GuGu {
    std::shared_ptr<InputKeyManager> InputKeyManager::m_instance;
	InputKeyManager& InputKeyManager::Get()
	{
		if (!m_instance)
			m_instance = std::make_shared<InputKeyManager>();
		return *m_instance;
	}
	void InputKeyManager::initKeyMappings()
	{
		static const uint32_t maxKeyMappings(256);

		uint32_t keyCodes[maxKeyMappings], charCodes[maxKeyMappings];
		GuGuUtf8Str keyNames[maxKeyMappings], charKeyNames[maxKeyMappings];

		const uint32_t charKeyMapSize(PlatformInput::getCharKeyMap(charCodes, charKeyNames, maxKeyMappings));//把平台无关，比如字母ABC等映射到字符串名
		const uint32_t keyMapSize(PlatformInput::getKeyMap(keyCodes, keyNames, maxKeyMappings));//把平台相关的虚拟键映射到字符串名

		m_keyMapCharToEnum.clear();
		m_keyMapVirtualToEnum.clear();

		for (uint32_t index = 0; index < keyMapSize; ++index)
		{
			Key key(keyNames[index]);

			//虚拟键和字符串名
			m_keyMapVirtualToEnum.insert({ keyCodes[index], key });
		}

		for (uint32_t index = 0; index < charKeyMapSize; ++index)
		{
			const Key key(charKeyNames[index]);

			//A、B、C等平台无关的键的字符串名
			m_keyMapCharToEnum.insert({ charCodes[index], key });
		}
	}
	Key InputKeyManager::getKeyFromCodes(const uint32_t keyCode, const uint32_t charCode) const
	{
		auto it = m_keyMapVirtualToEnum.find(keyCode);
		if (it != m_keyMapVirtualToEnum.end())
		{
			return it->second;
		}
		auto it2 = m_keyMapCharToEnum.find(charCode);
		if (it2 != m_keyMapCharToEnum.end())
		{
			return it2->second;
		}
		return Key("");
	}
}