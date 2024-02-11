#include <pch.h>

#include "WidgetGeometry.h"

namespace GuGu {
	WidgetGeometry::WidgetGeometry()
		: mLocalPosition(0.0, 0.0)
		, mLocalSize(0.0, 0.0)
		, mAbsoluteScale(1.0, 1.0)
		, mAbsolutePosition(0.0, 0.0)
		, mAccumulateTransform(math::float3x3::identity())
	{
	}
	WidgetGeometry::~WidgetGeometry()
	{
	}
	void WidgetGeometry::setLocalSize(math::double2 localSize)
	{
		mLocalSize = localSize;
	}
	math::double2 WidgetGeometry::getAbsolutePosition() const
	{
		return mAbsolutePosition;
	}
	math::double2 WidgetGeometry::getLocalSize() const
	{
		return mLocalSize;
	}
}