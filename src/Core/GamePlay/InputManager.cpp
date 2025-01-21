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
    void InputManager::updatePreviousMouseState() //将当前状态保存到过去的状态，用于计算鼠标偏移
    {
        m_previousMouseState = m_currentMouseState;
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

        //m_previousMouseState[key.getKeyName()] = m_currentMouseState[key.getKeyName()];
		m_currentMouseState[key.getKeyName()] = mouseState;
    }
    void InputManager::updateKeyboard(const Key& key, bool bPressed)
    {
        if (m_previousState.find(key.getKeyName()) == m_previousState.end())
            m_previousState[key.getKeyName()] = false;

		if (m_currentState.find(key.getKeyName()) == m_currentState.end())
            m_currentState[key.getKeyName()] = false;

        bool currentState = bPressed;

        //GuGu_LOGD("%d", currentState);
        //GuGu_LOGD("%s", key.getKeyName().getStr());
		//Event event;
		//event.m_type = currentState ? Event::EventType::PRESS : Event::EventType::RELEASE;
		//event.m_keyCode = key;
		//m_events.push_back(event);
        m_currentState[key.getKeyName()] = currentState;
    }
    void InputManager::updateWheelDelta(float delta)
    {
        m_wheelDelta = delta;
    }
    float InputManager::getWheelDelta() const
    {
        return m_wheelDelta;
    }
    bool InputManager::isKeyDown(const Key& key)
    {
		//for (const auto& item : m_events)
		//{
		//	if (item.m_keyCode == key.getKeyName() && item.m_type == Event::EventType::PRESS)
		//	{
		//		return true;
		//	}
		//}
		//return false;
        GuGu_LOGD("%s %d", key.getKeyName().getStr(), m_currentState[key.getKeyName()]);
        return m_currentState[key.getKeyName()];
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
        //m_events.clear();
    }
    void InputManager::clearMouseStates()
    {
        m_currentMouseState.clear();
        m_previousMouseState.clear();
    }
    math::float2 InputManager::getMouseDelta()
    {
        math::float2 mouseDelta;
        GuGuUtf8Str keyName = Keys::Invalid.getKeyName();
        mouseDelta.x = m_currentMouseState[keyName].m_mouseX - m_previousMouseState[keyName].m_mouseX;
        mouseDelta.y = m_currentMouseState[keyName].m_mouseY - m_previousMouseState[keyName].m_mouseY;
        return mouseDelta;
    }
}