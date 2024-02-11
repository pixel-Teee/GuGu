#pragma once

#include <Renderer/MyMath.h>

namespace GuGu {
	class WidgetGeometry
	{
	public:
		WidgetGeometry();

		virtual ~WidgetGeometry();

		void setLocalSize(math::double2 localSize);

		math::double2 getAbsolutePosition() const;

		math::double2 getLocalSize() const;
	private:
		math::double2 mLocalPosition;
		math::double2 mLocalSize;
		math::double2 mAbsoluteScale;
		math::double2 mAbsolutePosition;
		math::float3x3 mAccumulateTransform;
	};
}