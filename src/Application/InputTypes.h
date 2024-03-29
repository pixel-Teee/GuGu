#pragma once

#include <unordered_map>

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	struct Key
	{
		Key() {

		}

		Key(const GuGuUtf8Str inName)
			: m_keyName(inName)
		{}

		friend bool operator==(const Key& keyA, const Key& keyB) { return keyA.m_keyName == keyB.m_keyName; }
	private:
		GuGuUtf8Str m_keyName;
	};

	struct Keys
	{
		static const Key BackSpace;
		static const Key Left;
		static const Key Right;
	};
	struct InputKeyManager
	{
	public:
		static InputKeyManager& Get();

		void initKeyMappings();

		InputKeyManager()
		{
			initKeyMappings();
		}

		Key getKeyFromCodes(const uint32_t keyCode, const uint32_t charCode) const;

		static std::shared_ptr<InputKeyManager> m_instance;
		std::unordered_map<uint32_t, Key> m_keyMapVirtualToEnum;
		std::unordered_map<uint32_t, Key> m_keyMapCharToEnum;
	};
}