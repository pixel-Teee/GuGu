#pragma once

#include <functional>

namespace GuGu {
	struct CallBackInfo
	{
		int32_t id = 0;
		//延迟调用的时候的触发点
		float m_triggerTimePoint;

		std::function<void(int32_t callbackId)> m_callback;
	};
	class Timer
	{
	public:
		Timer();

		virtual float GetTotalTime() const = 0;

		virtual float GetDeltaTime() const = 0;

		virtual void Tick() = 0;//calculate time

		virtual ~Timer() {}

		std::shared_ptr<CallBackInfo> registerCallback(float delaySeconds, std::function<void(int32_t)> inCallback);

		void removeCallback(int32_t callbackId);

		void removeCallback(std::shared_ptr<CallBackInfo> inCallback);

		void onCallCallback();
	private:
		std::vector<std::shared_ptr<CallBackInfo>> m_callbacks;

		static int32_t m_callbackId;
	};

	std::shared_ptr<Timer> CreateTimerFactory();
}