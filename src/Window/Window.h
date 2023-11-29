#pragma 

#include <memory>

namespace GuGu {
	class Window
	{
	public:
		//static void DeferGeneratePlatformWindow(std::shared_ptr<Window> pWindow);

		virtual void DeferGeneratePlatformWindow() = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}