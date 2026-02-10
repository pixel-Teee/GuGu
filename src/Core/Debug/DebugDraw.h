#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	//3d viewport draw
	struct DebugDraw
	{
		//draw ray
		static void drawRay(math::float3 rayBegin, math::float3 rayEnd, math::float3 rayColor);
	};
}