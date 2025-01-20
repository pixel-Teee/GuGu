#include <pch.h>

#include "InputManager.h"

namespace GuGu {
    InputManager::InputManager()
    {
    }
    InputManager::~InputManager()
    {
    }
    InputManager& InputManager::getInputManager()
    {
        static InputManager inputManager;
        return inputManager;
    }
    void InputManager::updateMouseButton(const Key& key, int32_t mouseX, int32_t mouseY, bool bPressed)
    {
		if (m_previousMouseState.find(key.getKeyName()) == m_previousMouseState.end())
			m_previousMouseState[key.getKeyName()] = InputManager::MouseState(key.getKeyName(), mouseX, mouseY, false);
		if (m_currentMouseState.find(key.getKeyName()) == m_currentMouseState.end())
			m_currentMouseState[key.getKeyName()] = InputManager::MouseState(key.getKeyName(), mouseX, mouseY, false);

        MouseState mouseState;
        mouseState.m_bPressed = bPressed;
        mouseState.m_mouseX = mouseX;
        mouseState.m_mouseY = mouseY;
        mouseState.m_key = key.getKeyName();

        m_previousMouseState[key.getKeyName()] = m_currentMouseState[key.getKeyName()];
		m_currentMouseState[key.getKeyName()] = mouseState;
    }
    void InputManager::updateKeyboard(const Key& key, bool bPressed)
    {
        if (m_previousState.find(key.getKeyName()) == m_previousState.end())
            m_previousState[key.getKeyName()] = false;

        bool currentState = bPressed;

		Event event;
		event.m_type = currentState ? Event::EventType::PRESS : Event::EventType::RELEASE;
		event.m_keyCode = key;
		m_events.push_back(event);

        m_previousState[key.getKeyName()] = currentState;
    }
    bool InputManager::isKeyDown(const Key& key)
    {
		for (const auto& item : m_events)
		{
			if (item.m_keyCode == key.getKeyName() && item.m_type == Event::EventType::PRESS)
			{
				return true;
			}
		}
        return false;
    }
    bool InputManager::isMouseDown(const Key& key)
    {
        for (const auto& item : m_currentMouseState)
        {
            if (item.second.m_key == key.getKeyName() && item.second.m_bPressed)
                return true;
        }
        return false;
    }
    void InputManager::clearEvents()
    {
        m_events.clear();
    }
    void InputManager::clearMouseStates()
    {
        m_currentMouseState.clear();
        m_previousMouseState.clear();
    }
}