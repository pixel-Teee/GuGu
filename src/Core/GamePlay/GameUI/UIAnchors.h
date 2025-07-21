#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct UIAnchors
	{
		math::float2 m_minimum;

		math::float2 m_maximum;

		UIAnchors()
		{
			m_minimum = math::float2(0, 0);
			m_maximum = math::float2(1, 1);
		}

		UIAnchors(math::float2 inMinimum, math::float2 inMaximum)
		: m_minimum(inMinimum), m_maximum(inMaximum)
		{}
	};
}