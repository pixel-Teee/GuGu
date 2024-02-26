#include <pch.h>

#include "WidgetGeometry.h"

namespace GuGu {
	WidgetGeometry::WidgetGeometry()
		: mLocalPosition(0.0, 0.0)
		, mLocalSize(0.0, 0.0)
		, mAbsoluteScale(1.0)
		, mAbsolutePosition(0.0, 0.0)
		, mAccumulateTransform(math::double2x2::identity(), math::double2(0.0, 0.0))
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
	void WidgetGeometry::setAbsoluteScale(float scale)
	{
		mAbsoluteScale = scale;
		mAccumulateTransform.m_linear = math::double2x2::diagonal(scale);
	}
	float WidgetGeometry::getAbsoluteScale() const
	{
		return mAbsoluteScale;
	}
	WidgetGeometry WidgetGeometry::getChildGeometry(math::double2 inLocalSize, math::double2 inTranslation, math::daffine2 parentAccumulateTransform)
	{
		math::daffine2 localLayoutTransform(math::double2x2::identity(), inTranslation);
		WidgetGeometry childGeometry;
		childGeometry.mAccumulateTransform = parentAccumulateTransform * localLayoutTransform;
		childGeometry.mAbsolutePosition = childGeometry.mAccumulateTransform.m_translation;
		childGeometry.mLocalPosition = localLayoutTransform.m_translation;
		childGeometry.mLocalSize = inLocalSize;
		childGeometry.mAbsoluteScale = childGeometry.mAccumulateTransform.m_linear[0][0];//fix:get scale factor
		return childGeometry;
	}
	math::daffine2 WidgetGeometry::getAccumulateTransform()
	{
		return mAccumulateTransform;
	}
}