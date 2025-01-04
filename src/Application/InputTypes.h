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
		static const Key Enter;
		static const Key Left;
		static const Key Right;
		static const Key LeftMouseButton;
		static const Key RightMouseButton;
		static const Key MiddleMouseButton;
		static const Key ThumbMouseButton;
		static const Key ThumbMouseButton2;

		static const Key A;
		static const Key C;
		static const Key V;
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