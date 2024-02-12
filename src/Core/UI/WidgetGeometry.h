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

		WidgetGeometry getChildGeometry(math::double2 inLocalSize, math::double2 inTranslation, math::daffine2 parentAccumulateTransform);

		math::daffine2 getAccumulateTransform();
	private:
		math::double2 mLocalPosition;
		math::double2 mLocalSize;
		math::double2 mAbsoluteScale;
		math::double2 mAbsolutePosition;
		math::daffine2 mAccumulateTransform;
	};
}