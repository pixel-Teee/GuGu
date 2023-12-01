#pragma 

#include <memory>

namespace GuGu {
	class Window
	{
	public:
		//static void DeferGeneratePlatformWindow(std::shared_ptr<Window> pWindow);

		virtual void ToGeneratePlatformWindow() = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}