#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class WidgetGeometry
	{
	public:
		WidgetGeometry();

		virtual ~WidgetGeometry();

		void setLocalSize(math::double2 localSize);

		math::double2 getLocalSize() const;

		void setAbsoluteScale(float scale);

		float getAbsoluteScale() const;

		math::double2 getAbsolutePosition() const;

		math::daffine2 getAccumulateTransform();

		WidgetGeometry getChildGeometry(math::double2 inLocalSize, math::double2 inTranslation, math::daffine2 parentAccumulateTransform);

		WidgetGeometry getOffsetGeometry(math::double2 inTranslation);

		math::box2 getRenderBoundingRect() const;

		math::box2 getRenderBoundingRect(const math::box2& localSpaceRect) const;
	private:
		math::double2 mLocalPosition;
		math::double2 mLocalSize;
		float mAbsoluteScale;
		math::double2 mAbsolutePosition;
		math::daffine2 mAccumulateTransform;
	};
}