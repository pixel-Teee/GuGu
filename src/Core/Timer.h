#pragma once

namespace GuGu {
	class Timer
	{
	public:
		virtual float GetTotalTime() const = 0;

		virtual float GetDeltaTime() const = 0;

		virtual void Tick() = 0;//calculate time

		virtual ~Timer() {}
	};

	std::shared_ptr<Timer> CreateTimerFactory();
}