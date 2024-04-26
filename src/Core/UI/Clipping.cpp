#include <pch.h>

#include "Clipping.h"

namespace GuGu {
	ClippingZone::ClippingZone(const WidgetGeometry& boundingGeometry)
		: m_bIntersect(true)
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
	ClippingZone::ClippingZone(const math::box2& axisAlignedRect)
		: m_bIntersect(true)
	{
		math::float2 corner[4];//left top, right top, bottom left, bottom right
		axisAlignedRect.getCorners(corner);

		m_topLeft = corner[0];
		m_topRight = corner[1];
		m_bottomLeft = corner[2];
		m_bottomRight = corner[3];
	}
	ClippingZone ClippingZone::intersect(const ClippingZone& other) const
	{
		//todo:check axis aligned

		math::box2 Intersected(
			math::float2(std::max(m_topLeft.x, other.m_topLeft.x), std::max(m_topLeft.y, other.m_topLeft.y)),
			math::float2(std::min(m_bottomRight.x, other.m_bottomRight.x), std::min(m_bottomRight.y, other.m_bottomRight.y))
		);

		math::float2 corner[4];//left top, right top, bottom left, bottom right
		Intersected.getCorners(corner);

		//bottom < top || right < left
		if ((corner[2].y < corner[1].y) || (corner[1].x < corner[0].x))
		{
			return ClippingZone(math::box2(math::float2(0, 0), math::float2(0, 0)));
		}
		else
		{
			return ClippingZone(Intersected);
		}
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
	const ClippingState* ClippingManager::getPreviousClippingState(bool bWillIntersectWithParent) const
	{
		const ClippingState* previousClippingState = nullptr;

		if (!bWillIntersectWithParent)
		{
			for (int32_t stackIndex = m_clippingStack.size() - 1; stackIndex >= 0; --stackIndex)
			{
				const ClippingState& clippingState = m_clippingStates[m_clippingStack[stackIndex]];

				//todo:修复
				break;
			}
		}
		else if(m_clippingStack.size() > 0)
		{
			previousClippingState = &m_clippingStates[m_clippingStack.back()];//最后一个最近的
		}

		return previousClippingState;
	}
	ClippingState ClippingManager::createClippingState(const ClippingZone& inClipRect) const
	{
		const ClippingState* previousClippingState = getPreviousClippingState(inClipRect.getShouldIntersectParent());

		ClippingState newClippingState;

		if (previousClippingState == nullptr)
		{
			//axis aligned
			newClippingState.m_scissorRect = inClipRect;
		}
		else
		{
			newClippingState.m_scissorRect = previousClippingState->m_scissorRect->intersect(inClipRect);
		}

		return newClippingState;
	}

}