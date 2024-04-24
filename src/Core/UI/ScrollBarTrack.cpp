#include <pch.h>

#include "ScrollBarTrack.h"

#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	ScrollBarTrack::ScrollBarTrack()
	{
	}
	ScrollBarTrack::~ScrollBarTrack()
	{
	}
	void ScrollBarTrack::init(const BuilderArguments& arguments)
	{
		m_offsetFraction = 0;
		m_thumbSizeFraction = 1.0f;
		m_minThumbSize = 35.0f;
		m_orientation = arguments.morientation;

		m_childrens.push_back(arguments.mTopSlot);
		m_childrens.push_back(arguments.mBottomSlot);
		m_childrens.push_back(arguments.mThumbSlot);
	}
	math::float2 ScrollBarTrack::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_orientation == Orientation::Horizontal)
		{
			const float desiredHeight = std::max(std::max(m_childrens[0]->getChildWidget()->getFixedSize().y, m_childrens[1]->getChildWidget()->getFixedSize().y), m_childrens[2]->getChildWidget()->getFixedSize().y);
			return math::float2(m_minThumbSize, desiredHeight);
		}
		else
		{
			const float desiredWidth = std::max(std::max(m_childrens[0]->getChildWidget()->getFixedSize().x, m_childrens[1]->getChildWidget()->getFixedSize().x), m_childrens[2]->getChildWidget()->getFixedSize().x);
			return math::float2(m_minThumbSize, desiredWidth);
		}
	}
	void ScrollBarTrack::AllocationChildActualSpace(const WidgetGeometry& allottedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		const float width = allottedGeometry.mLocalSize.x;
		const float height = allottedGeometry.mLocalSize.y;

		//我们只需要展示所有三个儿子 ，当thumb可见的时候 ，否则我们只需要去显示track
		if (isNeeded())
		{
			TrackSizeInfo trackSizeInfo = getTrackSizeInfo(allottedGeometry);

			//安排 track 的上半部分
			math::float2 childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbStart, height)
				: math::float2(width, trackSizeInfo.m_thumbStart);

			math::float2 childPos(0, 0);
			WidgetGeometry childGeometry = allottedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[TOP_SLOT_INDEX]->getChildWidget());

			//安排 track 的下半部分
			childPos = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.getThumbEnd(), 0)
				: math::float2(0, trackSizeInfo.getThumbEnd());

			childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(allottedGeometry.getLocalSize().x - trackSizeInfo.getThumbEnd(), height)
				: math::float2(width, allottedGeometry.getLocalSize().y - trackSizeInfo.getThumbEnd());

			childGeometry = allottedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[BOTTOM_SLOT_INDEX]->getChildWidget());

			//安排 thumb
			childPos = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbStart, 0)
				: math::float2(0, trackSizeInfo.m_thumbStart);

			childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbSize, height)
				: math::float2(width, trackSizeInfo.m_thumbSize);

			childGeometry = allottedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[THUMB_SLOT_INDEX]->getChildWidget());
		}
		else
		{
			WidgetGeometry childGeometry = allottedGeometry.getChildGeometry(math::float2(width, height), math::float2(0, 0));
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[TOP_SLOT_INDEX]->getChildWidget());
		}
	}
	uint32_t ScrollBarTrack::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		uint32_t maxLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer);
				maxLayerId = std::max(maxLayerId, curWidgetsMaxLayerId);
			}
		}

		return maxLayerId;
	}
	SlotBase* ScrollBarTrack::getSlot(uint32_t index)
	{
		return m_childrens[index].get();
	}
	uint32_t ScrollBarTrack::getSlotsNumber()
	{
		return m_childrens.size();
	}
	bool ScrollBarTrack::isNeeded() const
	{
		return m_thumbSizeFraction < (1.0f - 1.e-4f) || m_bIsAlwaysVisible;
	}
	ScrollBarTrack::TrackSizeInfo ScrollBarTrack::getTrackSizeInfo(const WidgetGeometry& inTrackGeometry) const
	{
		const float currentMinThumbSize = m_thumbSizeFraction <= 0.0f ? 0.0f : m_minThumbSize;
		return TrackSizeInfo(inTrackGeometry, m_orientation, currentMinThumbSize, this->m_thumbSizeFraction, this->m_offsetFraction);
	}
}