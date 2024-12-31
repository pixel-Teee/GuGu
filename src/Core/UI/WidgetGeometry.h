#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class WidgetGeometry
	{
	public:
		WidgetGeometry();

		virtual ~WidgetGeometry();

		//void setLocalSize(math::float2 localSize);

		math::float2 getLocalSize() const;

		void setAbsoluteScale(float scale);

		float getAbsoluteScale() const;

		math::float2 getAbsolutePosition() const;

		math::float2 getAbsoluteSize() const;

		math::box2 getAbsoluteRect() const;

		math::affine2 getAccumulateLayoutTransform() const;

		math::affine2 getAccumulateRenderTransform() const;

		WidgetGeometry getChildGeometry(const math::float2& inLocalSize, const math::affine2& inLocalLayoutTransform, const math::affine2& inLocalRenderTransform, const math::float2& inLocalRenderTransformPivot) const;

		WidgetGeometry getChildGeometry(math::float2 inLocalSize, math::float2 inTranslation) const;

		WidgetGeometry getOffsetGeometry(math::float2 inTranslation) const;

		math::float2 absoluteToLocal(math::float2 absolutePosition) const;

		math::box2 getRenderBoundingRect() const;

		math::box2 getRenderBoundingRect(const math::box2& localSpaceRect) const;
		
		static WidgetGeometry makeRoot(const math::float2& inLocalSize, const math::affine2& inLocalLayoutTransform);

	public:
		math::affine2 mAccumulateRenderTransform;//相比于 accumulate layout transform ，多了旋转、斜切
		math::float2 mLocalPosition;//local layout transform 导出
		math::float2 mLocalSize;
		float mAbsoluteScale;//从accumulate layout transform 导出
		math::float2 mAbsolutePosition;//从accumulate layout transform 导出
		math::affine2 mAccumulateLayoutTransform;//accumulate layout transform 只存放绝对缩放和绝对空间，由 absolute position 和 absolute scale 组成
	};
}