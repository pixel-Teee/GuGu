#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class Window
	{
	public:
		virtual void ToGeneratePlatformWindow() = 0;

		virtual float getDpiFactor() = 0;

		virtual math::float2 getWindowScreenSpacePosition() = 0;

		virtual void moveWindowTo(math::float2 newPosition) = 0;
	};

	std::shared_ptr<Window> CreateWindowFactory();
}