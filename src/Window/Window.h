#pragma 

#include <memory>

#include <Renderer/MyMath.h> //todo:fix this

namespace GuGu {
	class Window
	{
	public:
		//static void DeferGeneratePlatformWindow(std::shared_ptr<Window> pWindow);

		virtual void ToGeneratePlatformWindow() = 0;

		//virtual math::int2 getSize() = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}