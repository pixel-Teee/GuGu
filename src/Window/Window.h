#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class WindowWidget;
	class Window
	{
	public:
		virtual void ToGeneratePlatformWindow(std::shared_ptr<WindowWidget> inWindowWidget) = 0;

		virtual float getDpiFactor() = 0;

		virtual math::float2 getWindowScreenSpacePosition() = 0;

		virtual void moveWindowTo(math::float2 newPosition) = 0;

		virtual void reshapeWindow(math::float2 newPosition, math::float2 newSize) = 0;

		virtual void destroy() = 0;

		virtual void setWindowFocus() = 0;

		virtual void show() = 0;

		virtual void hide() = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}