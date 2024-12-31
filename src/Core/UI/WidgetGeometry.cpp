#include <pch.h>

#include "WidgetGeometry.h"

namespace GuGu {
	WidgetGeometry::WidgetGeometry()
		: mLocalPosition(0.0, 0.0)
		, mLocalSize(0.0, 0.0)
		, mAbsoluteScale(1.0)
		, mAbsolutePosition(0.0, 0.0)
		, mAccumulateLayoutTransform(math::float2x2::identity(), math::float2(0.0, 0.0))
		, mAccumulateRenderTransform(math::float2x2::identity(), math::float2(0.0, 0.0))
	{
	}
	WidgetGeometry::~WidgetGeometry()
	{
	}
	//void WidgetGeometry::setLocalSize(math::float2 localSize)
	//{
	//	mLocalSize = localSize;
	//}
	math::float2 WidgetGeometry::getLocalSize() const
	{
		return mLocalSize;
	}
	math::float2 WidgetGeometry::getAbsolutePosition() const
	{
		return mAbsolutePosition;
	}
	math::float2 WidgetGeometry::getAbsoluteSize() const
	{
		return mAccumulateRenderTransform.transformVector(mLocalSize);
	}

	math::box2 WidgetGeometry::getAbsoluteRect() const
	{
		math::box2 rect;
		rect.m_maxs = mAccumulateLayoutTransform.transformPoint(mLocalSize);
		rect.m_mins = mAccumulateLayoutTransform.transformPoint(math::float2(0, 0));
		return rect;
	}

	void WidgetGeometry::setAbsoluteScale(float scale)
	{
		mAbsoluteScale = scale;
		mAccumulateLayoutTransform.m_linear = math::float2x2::diagonal(scale);
	}
	float WidgetGeometry::getAbsoluteScale() const
	{
		return mAbsoluteScale;
	}
	GuGu::math::affine2 WidgetGeometry::getAccumulateLayoutTransform() const
{
		return mAccumulateLayoutTransform;
	}
	math::affine2 WidgetGeometry::getAccumulateRenderTransform() const
	{
		return mAccumulateRenderTransform;
	}
	//pivot 是个[0, 1] 范围之间的东西
	WidgetGeometry WidgetGeometry::getChildGeometry(const math::float2& inLocalSize, const math::affine2& inLocalLayoutTransform, const math::affine2& inLocalRenderTransform, const math::float2& inLocalRenderTransformPivot) const
	{
		math::affine2 localLayoutTransform = inLocalLayoutTransform;
		WidgetGeometry childGeometry;
		childGeometry.mAccumulateLayoutTransform = localLayoutTransform * mAccumulateLayoutTransform;//this order is important

		//------计算accumulate render transform------
		
		//1.转换锚点到局部空间，并且使得它为原点
		math::float2 origin = -(inLocalSize * inLocalRenderTransformPivot);

		//2.应用 render transform 以围绕锚点为中心
		math::affine2 tempAccumulateRenderTransform = math::affine2(inLocalRenderTransform.m_linear, inLocalRenderTransform.m_linear * origin + inLocalRenderTransform.m_translation);

		//3.转换锚点回去
		math::affine2 temp2AccumulateRenderTransform = math::affine2(tempAccumulateRenderTransform.m_linear, tempAccumulateRenderTransform.m_translation + inLocalSize * inLocalRenderTransformPivot);

		//4.应用layout transform
		math::affine2 temp3AccumulateRenderTansform = temp2AccumulateRenderTransform * inLocalLayoutTransform;

		//5.最终应用父 accumulate transform
		childGeometry.mAccumulateRenderTransform = temp3AccumulateRenderTansform * mAccumulateRenderTransform;

		//------计算accumulate render transform------
		childGeometry.mAbsolutePosition = childGeometry.mAccumulateLayoutTransform.m_translation;
		childGeometry.mLocalPosition = localLayoutTransform.m_translation;
		childGeometry.mLocalSize = inLocalSize;
		childGeometry.mAbsoluteScale = childGeometry.mAccumulateLayoutTransform.m_linear[0][0];//accumulate transform 只有缩放因子，直接从第一个分量取出来
		return childGeometry;
	}
	WidgetGeometry WidgetGeometry::getChildGeometry(math::float2 inLocalSize, math::float2 inTranslation) const
	{
		math::affine2 localLayoutTransform(math::float2x2::identity(), inTranslation);
		WidgetGeometry childGeometry;
		childGeometry.mAccumulateLayoutTransform = localLayoutTransform * mAccumulateLayoutTransform;//this order is important
		childGeometry.mAccumulateRenderTransform = localLayoutTransform * mAccumulateRenderTransform;
		childGeometry.mAbsolutePosition = childGeometry.mAccumulateLayoutTransform.m_translation;
		childGeometry.mLocalPosition = localLayoutTransform.m_translation;
		childGeometry.mLocalSize = inLocalSize;
		childGeometry.mAbsoluteScale = childGeometry.mAccumulateLayoutTransform.m_linear[0][0];//accumulate transform 只有缩放因子，直接从第一个分量取出来
		return childGeometry;
	}
	WidgetGeometry WidgetGeometry::getOffsetGeometry(math::float2 inTranslation) const
	{
		WidgetGeometry offsetGeometry = *this;
		offsetGeometry.mAccumulateLayoutTransform.m_translation += inTranslation;
		offsetGeometry.mAbsolutePosition = offsetGeometry.mAccumulateLayoutTransform.m_translation;
		return offsetGeometry;
	}
	math::float2 WidgetGeometry::absoluteToLocal(math::float2 absolutePosition) const
	{
		math::affine2 inverseTransform = math::inverse(mAccumulateLayoutTransform);
		return inverseTransform.transformPoint(absolutePosition);
	}
	math::box2 WidgetGeometry::getRenderBoundingRect() const
	{
		math::box2 localRect = math::box2(math::float2(0.0f, 0.0f), math::float2(mLocalSize.x, mLocalSize.y));
		return getRenderBoundingRect(localRect);
	}
	math::box2 WidgetGeometry::getRenderBoundingRect(const math::box2& localSpaceRect) const
	{
		math::box2 rect;
		math::float2 topLeft = mAccumulateLayoutTransform.transformPoint(math::float2(localSpaceRect.m_mins.x, localSpaceRect.m_mins.y));
		rect.m_mins = math::float2(topLeft.x, topLeft.y);
		math::float2 extent = mAccumulateLayoutTransform.transformVector(math::float2(localSpaceRect.m_maxs.x, localSpaceRect.m_maxs.y));
		rect.m_maxs = math::float2(extent.x, extent.y);
		return rect;
	}
	WidgetGeometry WidgetGeometry::makeRoot(const math::float2& inLocalSize, const math::affine2& inLocalLayoutTransform)
	{
		WidgetGeometry root;
		
		root.mAccumulateLayoutTransform = inLocalLayoutTransform;
		root.mAccumulateRenderTransform = inLocalLayoutTransform;
		root.mAbsolutePosition = root.mAccumulateLayoutTransform.m_translation;
		root.mLocalPosition = inLocalLayoutTransform.m_translation;
		root.mLocalSize = inLocalSize;
		root.mAbsoluteScale = root.mAccumulateLayoutTransform.m_linear[0][0];

		return root;
	}

}