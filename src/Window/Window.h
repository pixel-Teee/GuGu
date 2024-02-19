#pragma once

namespace GuGu {
	class Window
	{
	public:
		virtual void ToGeneratePlatformWindow() = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}