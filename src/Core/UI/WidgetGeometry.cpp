#include <pch.h>

#include "WidgetGeometry.h"

namespace GuGu {
	WidgetGeometry::WidgetGeometry()
		: mLocalPosition(0.0, 0.0)
		, mLocalSize(0.0, 0.0)
		, mAbsoluteScale(1.0)
		, mAbsolutePosition(0.0, 0.0)
		, mAccumulateTransform(math::float2x2::identity(), math::float2(0.0, 0.0))
	{
	}
	WidgetGeometry::~WidgetGeometry()
	{
	}
	void WidgetGeometry::setLocalSize(math::float2 localSize)
	{
		mLocalSize = localSize;
	}
	math::float2 WidgetGeometry::getLocalSize() const
	{
		return mLocalSize;
	}
	math::float2 WidgetGeometry::getAbsolutePosition() const
	{
		return mAbsolutePosition;
	}	
	void WidgetGeometry::setAbsoluteScale(float scale)
	{
		mAbsoluteScale = scale;
		mAccumulateTransform.m_linear = math::float2x2::diagonal(scale);
	}
	float WidgetGeometry::getAbsoluteScale() const
	{
		return mAbsoluteScale;
	}
	math::affine2 WidgetGeometry::getAccumulateTransform() const
	{
		return mAccumulateTransform;
	}
	WidgetGeometry WidgetGeometry::getChildGeometry(math::float2 inLocalSize, math::float2 inTranslation, const math::affine2 parentAccumulateTransform) const
	{
		math::affine2 localLayoutTransform(math::float2x2::identity(), inTranslation);
		WidgetGeometry childGeometry;
		childGeometry.mAccumulateTransform = localLayoutTransform * parentAccumulateTransform;//this order is important
		childGeometry.mAbsolutePosition = childGeometry.mAccumulateTransform.m_translation;
		childGeometry.mLocalPosition = localLayoutTransform.m_translation;
		childGeometry.mLocalSize = inLocalSize;
		childGeometry.mAbsoluteScale = childGeometry.mAccumulateTransform.m_linear[0][0];//fix:to get scale factor, to use meaningful name
		return childGeometry;
	}
	WidgetGeometry WidgetGeometry::getOffsetGeometry(math::float2 inTranslation) const
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
		math::float2 topLeft = mAccumulateTransform.transformPoint(math::float2(localSpaceRect.m_mins.x, localSpaceRect.m_mins.y));
		rect.m_mins = math::float2(topLeft.x, topLeft.y);
		math::float2 extent = mAccumulateTransform.transformVector(math::float2(localSpaceRect.m_maxs.x, localSpaceRect.m_maxs.y));
		rect.m_maxs = math::float2(extent.x, extent.y);
		return rect;
	}
}