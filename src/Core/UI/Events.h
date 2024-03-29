#pragma once

#include <Core/Math/MyMath.h>
#include <Core/GuGuUtf8Str.h>

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

	struct CharacterEvent : public InputEvent
	{
		CharacterEvent(const GuGuUtf8Str inCharacter)
			: m_character(inCharacter)
		{}

		GuGuUtf8Str getCharacter() const
		{
			return m_character;
		}
	private:
		GuGuUtf8Str m_character;
	};
}