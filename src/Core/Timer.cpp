#include <pch.h>

#include "Timer.h"

namespace GuGu {

	Timer::Timer()
	{
		m_callbackId = 0;
	}

	std::shared_ptr<CallBackInfo> Timer::registerCallback(float delaySeconds, std::function<void(int32_t)> inCallback)
	{
		std::shared_ptr<CallBackInfo> callBackInfoPtr = std::make_shared<CallBackInfo>();
		callBackInfoPtr->m_triggerTimePoint = GetTotalTime() + delaySeconds;
		callBackInfoPtr->m_callback = inCallback;
		m_callbackId = ++m_callbackId;
		callBackInfoPtr->id = m_callbackId;
		m_callbacks.push_back(callBackInfoPtr);
		return callBackInfoPtr;
	}

	void Timer::removeCallback(int32_t callbackId)
	{
		int32_t pos = -1;
		for (int32_t i = 0; i < m_callbacks.size(); ++i)
		{
			if (m_callbacks[i]->id == callbackId)
			{
				pos = i;
				break;
			}
		}
		m_callbacks.erase(m_callbacks.begin() + pos);
	}

	void Timer::removeCallback(std::shared_ptr<CallBackInfo> inCallback)
	{
		int32_t pos = -1;
		for (int32_t i = 0; i < m_callbacks.size(); ++i)
		{
			if (m_callbacks[i] == inCallback)
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
			if (m_callbacks[i]->m_triggerTimePoint <= GetTotalTime())
				m_callbacks[i]->m_callback(m_callbacks[i]->id);
		}
	}

	int32_t Timer::m_callbackId = 0;
}