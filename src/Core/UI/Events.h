#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

#include <Application/InputTypes.h>

namespace GuGu {
	//修饰键
	class ModifierKeysState
	{
	public:
		ModifierKeysState()
		{
			m_bIsLeftControlDown = false;
			m_bIsLeftShitDown = false;
		}
		ModifierKeysState(const bool bInLeftControlDown, const bool bInLeftShitDown)
			: m_bIsLeftControlDown(bInLeftControlDown)
			, m_bIsLeftShitDown(bInLeftShitDown)
		{}

		bool isControlDown() const
		{
			return m_bIsLeftControlDown;
		}

		bool isShiftDown() const
		{
			return m_bIsLeftShitDown;
		}

		uint16_t m_bIsLeftControlDown : 1;
		uint16_t m_bIsLeftShitDown : 1;
	};

	struct InputEvent
	{
	public:
		InputEvent()
		{}
		InputEvent(const ModifierKeysState& inModifierKeys)
			: m_modifierKeys(inModifierKeys)
		{}

		bool isControlDown() const
		{
			return m_modifierKeys.isControlDown();
		}

		bool isShiftDown() const
		{
			return m_modifierKeys.isShiftDown();
		}
	protected:
		//修饰键
		ModifierKeysState m_modifierKeys;
	};

	struct PointerEvent : public InputEvent
	{
	public:
		PointerEvent(const math::float2& inScreenSpacePosition,
			const math::float2& inLastScreenSpacePosition)
			: m_screenSpacePosition(inScreenSpacePosition)
			, m_lastScreenSpacePosition(inLastScreenSpacePosition)
			, m_cursorDelta(inScreenSpacePosition - inLastScreenSpacePosition)
		{}

		math::float2 getCursorDelta() const
		{
			return m_cursorDelta;
		}
		math::float2 m_screenSpacePosition;
		math::float2 m_lastScreenSpacePosition;
		math::float2 m_cursorDelta;
	};

	struct CharacterEvent : public InputEvent
	{
		CharacterEvent(const GuGuUtf8Str inCharacter)
			: m_character(inCharacter)
		{}

		GuGuUtf8Str getCharacter() const
		{
			return m_character;
		}
	private:
		GuGuUtf8Str m_character;
	};

	struct KeyEvent : public InputEvent
	{
	public:
		KeyEvent(const Key inKey, const ModifierKeysState& inModifierKeys, const uint32_t inCharacterCode, const int32_t inKeyCode)
			: m_key(inKey)
			, InputEvent(inModifierKeys)
			, m_characterCode(inCharacterCode)
			, m_keyCode(inKeyCode)
		{}

		Key getKey() const
		{
			return m_key;
		}
	private:

		//按下的键的名字
		Key m_key;
		//按下的字母码点
		uint32_t m_characterCode;
		//原先从键盘输入的键盘码在任何转换映射之前
		uint32_t m_keyCode;
	};
}