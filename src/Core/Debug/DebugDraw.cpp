#include <pch.h>

#include "DebugDraw.h"

#include <Application/Application.h>
#include <Renderer/Renderer.h>
#include <Renderer/Demo.h>

namespace GuGu {

	void DebugDraw::drawRay(math::float3 rayBegin, math::float3 rayEnd, math::float3 rayColor)
	{
		std::shared_ptr<Application> app = Application::getApplication();
		Demo* demo = app->getRenderer()->getDemoPass();
		if (demo)
		{
			demo->addDebugPhysicsInfo(rayBegin, rayEnd, math::float4(rayColor, 1.0f));
		}
	}

}