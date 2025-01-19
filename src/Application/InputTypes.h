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

		const GuGuUtf8Str& getKeyName() const;

		friend bool operator==(const Key& keyA, const Key& keyB) { return keyA.m_keyName == keyB.m_keyName; }
	private:
		GuGuUtf8Str m_keyName;
	};

	struct Keys
	{
		static const Key BackSpace;
		static const Key Tab;
		static const Key Enter;
		static const Key Pause;

		static const Key Left;
		static const Key Right;
		static const Key Down;
		static const Key Up;

		static const Key LeftMouseButton;
		static const Key RightMouseButton;
		static const Key MiddleMouseButton;
		static const Key ThumbMouseButton;
		static const Key ThumbMouseButton2;

		static const Key A;
		static const Key B;
		static const Key C;
		static const Key D;
		static const Key E;
		static const Key F;
		static const Key G;
		static const Key H;
		static const Key I;
		static const Key J;
		static const Key K;
		static const Key L;
		static const Key M;
		static const Key N;
		static const Key O;
		static const Key P;
		static const Key Q;
		static const Key R;
		static const Key S;
		static const Key T;
		static const Key U;
		static const Key V;
		static const Key W;
		static const Key X;
		static const Key Y;
		static const Key Z;

		static const Key Zero;
		static const Key One;
		static const Key Two;
		static const Key Three;
		static const Key Four;
		static const Key Five;
		static const Key Six;
		static const Key Seven;
		static const Key Eight;
		static const Key Nine;

		static const Key LeftShift;
		static const Key RightShift;
		static const Key LeftControl;
		static const Key RightControl;
		static const Key LeftAlt;
		static const Key RightAlt;
		static const Key LeftCommand;
		static const Key RightCommand;

		static const Key Invalid;
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