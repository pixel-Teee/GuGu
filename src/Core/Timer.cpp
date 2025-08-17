#include <pch.h>

#include "Timer.h"

namespace GuGu {

	Timer::Timer()
	{
		m_callbackId = 0;
	}

	void Timer::registerCallback(int32_t delaySeconds, std::function<void(int32_t)> inCallback)
	{
		CallBackInfo callbackInfo;
		callbackInfo.m_triggerTimePoint = GetTotalTime() + delaySeconds;
		callbackInfo.m_callback = inCallback;
		m_callbackId = ++m_callbackId;
		callbackInfo.id = m_callbackId;
		m_callbacks.push_back(callbackInfo);
	}

	void Timer::removeCallback(int32_t callbackId)
	{
		int32_t pos = -1;
		for (int32_t i = 0; i < m_callbacks.size(); ++i)
		{
			if (m_callbacks[i].id == callbackId)
			{
				pos = i;
				break;
			}
		}
		m_callbacks.erase(m_callbacks.begin() + pos);
	}

	void Timer::onCallCallback()
	{
		for (int32_t i = 0; i < m_callbacks.size(); ++i)
		{
			if (m_callbacks[i].m_triggerTimePoint <= GetTotalTime())
				m_callbacks[i].m_callback(m_callbacks[i].id);
		}
	}

	int32_t Timer::m_callbackId = 0;
}