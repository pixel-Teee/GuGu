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
	math::double2 WidgetGeometry::getLocalSize() const
	{
		return mLocalSize;
	}
	math::double2 WidgetGeometry::getAbsolutePosition() const
	{
		return mAbsolutePosition;
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
	math::daffine2 WidgetGeometry::getAccumulateTransform()
	{
		return mAccumulateTransform;
	}
	WidgetGeometry WidgetGeometry::getChildGeometry(math::double2 inLocalSize, math::double2 inTranslation, math::daffine2 parentAccumulateTransform)
	{
		math::daffine2 localLayoutTransform(math::double2x2::identity(), inTranslation);
		WidgetGeometry childGeometry;
		childGeometry.mAccumulateTransform = parentAccumulateTransform * localLayoutTransform;
		childGeometry.mAbsolutePosition = childGeometry.mAccumulateTransform.m_translation;
		childGeometry.mLocalPosition = localLayoutTransform.m_translation;
		childGeometry.mLocalSize = inLocalSize;
		childGeometry.mAbsoluteScale = childGeometry.mAccumulateTransform.m_linear[0][0];//fix:to get scale factor
		return childGeometry;
	}
	WidgetGeometry WidgetGeometry::getOffsetGeometry(math::double2 inTranslation)
	{
		WidgetGeometry offsetGeometry = *this;
		offsetGeometry.mAccumulateTransform.m_translation += inTranslation;
		offsetGeometry.mAbsolutePosition = offsetGeometry.mAccumulateTransform.m_translation;
		return offsetGeometry;
	}
	math::box2 WidgetGeometry::getRenderBoundingRect() const
	{
		math::box2 localRect = math::box2(math::float2(0.0f, 0.0f), math::float2(mLocalSize.x, mLocalSize.y));
		return getRenderBoundingRect(localRect);
	}
	math::box2 WidgetGeometry::getRenderBoundingRect(const math::box2& localSpaceRect) const
	{
		math::box2 rect;
		math::double2 topLeft = mAccumulateTransform.transformPoint(math::double2(localSpaceRect.m_mins.x, localSpaceRect.m_mins.y));
		rect.m_mins = math::float2(topLeft.x, topLeft.y);
		math::double2 extent = mAccumulateTransform.transformVector(math::double2(localSpaceRect.m_maxs.x, localSpaceRect.m_maxs.y));
		rect.m_maxs = math::float2(extent.x, extent.y);
		return rect;
	}
}