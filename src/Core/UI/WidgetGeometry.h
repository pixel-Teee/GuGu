#pragma once

#include <Renderer/MyMath.h>

namespace GuGu {
	class WidgetGeometry
	{
	public:
		WidgetGeometry();

		virtual ~WidgetGeometry();

	private:
		math::double2 mLocalPosition;
		math::double2 mLocalSize;
		math::double2 mAbsoluteScale;
		math::double2 mAbsolutePosition;
		math::float3x3 mAccumulateTransform;
	};
}