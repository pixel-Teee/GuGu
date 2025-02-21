#pragma once

#include <Core/Math/MyMath.h>

#include "WidgetGeometry.h"

namespace GuGu {
	enum class WidgetClipping
	{
		Inherit, //don't clip

		ClipToBounds,

		OnDemand
	};
	class ClippingZone
	{
	public:
		math::float2 m_topLeft;
		math::float2 m_topRight;
		math::float2 m_bottomLeft;
		math::float2 m_bottomRight;

		explicit ClippingZone(const WidgetGeometry& boundingGeometry);

		explicit ClippingZone(const math::box2& axisAlignedRect);

		void setShouldIntersectParent(bool bValue)
		{
			m_bIntersect = bValue;
		}

		bool getShouldIntersectParent() const
		{
			return m_bIntersect;
		}

		ClippingZone intersect(const ClippingZone& other) const;

	private:
		uint8_t m_bIntersect : 1;
	};
	class ClippingState
	{
	public:
		ClippingState();

		~ClippingState();
	
		std::optional<ClippingZone> m_scissorRect;
	};
	class ClippingManager
	{
	public:
		ClippingManager();

		~ClippingManager();

		int32_t pushClippingState(const ClippingState& inClippingState);
		int32_t pushClip(const ClippingZone& inClippingZone);
		int32_t getClippingIndex() const;
		void popClip();
		void resetClippingState();
		const ClippingState* getClippingState(uint32_t inClippingIndex) const;

		const ClippingState* getPreviousClippingState(bool bWillIntersectWithParent) const;//获取之前的 clip state
	private:
		ClippingState createClippingState(const ClippingZone& inClipRect) const;

		std::vector<int32_t> m_clippingStack;

		std::vector<ClippingState> m_clippingStates;
	};
}