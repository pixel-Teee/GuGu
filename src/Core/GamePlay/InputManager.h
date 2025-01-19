#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <Application/InputTypes.h>

namespace GuGu {
	struct Event {
		enum EventType
		{
			PRESS,
			RELEASE
		};

		EventType m_type;

		Key m_keyCode;//keyboard events

		int32_t m_mouseX, m_mouseY;//mouse events
	};

	struct InputManager
	{
	public:
		InputManager();

		virtual ~InputManager();

		static InputManager& getInputManager();

		void updateMouseButton(const Key& key, int32_t mouseX, int32_t mouseY, bool bPressed);

		void updateKeyboard(const Key& key, bool bPressed);

		bool isKeyDown(const Key& key);

		bool isMouseDown(const Key& key);

		void clearEvents();

		struct MouseState
		{
			bool m_bPressed;
			GuGuUtf8Str m_key;//key name
			int32_t m_mouseX;
			int32_t m_mouseY;

			MouseState()
				: m_key(Keys::Invalid.getKeyName()), m_mouseX(0), m_mouseY(0), m_bPressed(false)
			{}

			MouseState(const GuGuUtf8Str& keyName, int32_t mouseX, int32_t mouseY, bool bPressed)
				: m_key(keyName), m_mouseX(mouseX), m_mouseY(mouseY), m_bPressed(bPressed)
			{}

			bool operator==(const MouseState& rhs) const
			{
				return m_key == rhs.m_key || m_mouseX == rhs.m_mouseX || m_mouseY == rhs.m_mouseY;
			}

			bool operator!=(const MouseState& rhs) const
			{
				return !operator==(rhs);
			}
		};
	private:
		std::map<GuGuUtf8Str, bool> m_previousState;

		std::map<GuGuUtf8Str, MouseState> m_previousMouseState;

		std::vector<Event> m_events;
	};
}