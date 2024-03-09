#pragma once

#include <Core/Math/MyMath.h>

#include "WidgetGeometry.h"

namespace GuGu {
	enum class WidgetClipping
	{
		Inherit, //don't clip

		ClipToBounds
	};
	class ClippingZone
	{
	public:
		math::float2 m_topLeft;
		math::float2 m_topRight;
		math::float2 m_bottomLeft;
		math::float2 m_bottomRight;

		explicit ClippingZone(const WidgetGeometry& boundingGeometry);
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
	private:
		ClippingState createClippingState(const ClippingZone& inClipRect) const;

		std::vector<int32_t> m_clippingStack;

		std::vector<ClippingState> m_clippingStates;
	};
}