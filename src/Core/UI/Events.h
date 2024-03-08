#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct InputEvent
	{

	};

	struct PointerEvent : public InputEvent
	{
	public:
		PointerEvent(const math::float2& inScreenSpacePosition,
			const math::float2& inLastScreenSpacePosition)
			: m_screenSpacePosition(inScreenSpacePosition)
			, m_lastScreenSpacePosition(inLastScreenSpacePosition)
			, m_cursorDelta(inScreenSpacePosition - inLastScreenSpacePosition)
		{}
		math::float2 m_screenSpacePosition;
		math::float2 m_lastScreenSpacePosition;
		math::float2 m_cursorDelta;
	};
}