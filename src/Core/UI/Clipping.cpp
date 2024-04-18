#include <pch.h>

#include "Clipping.h"

namespace GuGu {
	ClippingZone::ClippingZone(const WidgetGeometry& boundingGeometry)
	{
		//m_topLeft = math::float2(boundingGeometry.getAbsolutePosition().x, boundingGeometry.getAbsolutePosition().y);
		//m_topRight = math::float2(m_topLeft.x + boundingGeometry.getLocalSize().x, m_topLeft.y);
		//m_bottomLeft = math::float2(m_topLeft.x, m_topLeft.y + boundingGeometry.getLocalSize().y);
		//m_bottomRight = math::float2(m_topLeft.x + boundingGeometry.getLocalSize().x, m_topLeft.y + boundingGeometry.getLocalSize().y);

		math::float2 localSize = boundingGeometry.getLocalSize();
		math::affine2 transform = boundingGeometry.getAccumulateLayoutTransform();

		m_topLeft = transform.transformPoint(math::float2(0.0f, 0.0f));
		m_topRight = transform.transformPoint(math::float2(localSize.x, 0.0f));
		m_bottomLeft = transform.transformPoint(math::float2(0.0f, localSize.y));
		m_bottomRight = transform.transformPoint(math::float2(localSize.x, localSize.y));

		//m_topLeft = math::float2(0.0f, 0.0f);
		//m_topRight = math::float2(200.0f, 0.0f);
		//m_bottomLeft = math::float2(0.0f, 200.0f);
		//m_bottomRight = math::float2(200.0f, 200.0f);
	}
	ClippingState::ClippingState()
	{
	}
	ClippingState::~ClippingState()
	{
	}
	ClippingManager::ClippingManager()
	{
	}
	ClippingManager::~ClippingManager()
	{
	}
	int32_t ClippingManager::pushClippingState(const ClippingState& inClippingState)
	{
		m_clippingStates.push_back(inClippingState);

		int32_t newClippingStateIndex = m_clippingStates.size() - 1;

		m_clippingStack.push_back(newClippingStateIndex);

		return newClippingStateIndex;
	}
	int32_t ClippingManager::pushClip(const ClippingZone& inClippingZone)
	{
		return pushClippingState(createClippingState(inClippingZone));
	}
	int32_t ClippingManager::getClippingIndex() const
	{
		return m_clippingStack.size() > 0 ? m_clippingStack.back() : -1;
	}
	void ClippingManager::popClip()
	{
		if (m_clippingStack.size() > 0)
			m_clippingStack.pop_back();
	}
	void ClippingManager::resetClippingState()
	{
		m_clippingStates.clear();
		m_clippingStack.clear();
	}
	const ClippingState* ClippingManager::getClippingState(uint32_t inClippingIndex) const
	{
		if (inClippingIndex != -1)
			return &m_clippingStates[inClippingIndex];
		return nullptr;
	}
	ClippingState ClippingManager::createClippingState(const ClippingZone& inClipRect) const
	{
		ClippingState newClippingState;

		newClippingState.m_scissorRect = inClipRect;

		return newClippingState;
	}

}