#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class WidgetGeometry
	{
	public:
		WidgetGeometry();

		virtual ~WidgetGeometry();

		void setLocalSize(math::float2 localSize);

		math::float2 getLocalSize() const;

		void setAbsoluteScale(float scale);

		float getAbsoluteScale() const;

		math::float2 getAbsolutePosition() const;

		math::affine2 getAccumulateTransform();

		WidgetGeometry getChildGeometry(math::float2 inLocalSize, math::float2 inTranslation, math::affine2 parentAccumulateTransform);

		WidgetGeometry getOffsetGeometry(math::float2 inTranslation);

		math::box2 getRenderBoundingRect() const;

		math::box2 getRenderBoundingRect(const math::box2& localSpaceRect) const;
	private:
		math::float2 mLocalPosition;
		math::float2 mLocalSize;
		float mAbsoluteScale;
		math::float2 mAbsolutePosition;
		math::affine2 mAccumulateTransform;
	};
}