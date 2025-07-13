#include <pch.h>

#include "MyMath.h"

namespace GuGu {
	namespace math {

		GuGu::math::double3 Rotator::getRadians() const
		{
			float toRadians = math::PI_d / 180.0f;
			return math::double3(roll * toRadians, pitch * toRadians, yaw * toRadians);
		}

	}
}