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
		m_offsetFraction = 0; //thumb 的起始偏移所占的比例
		m_thumbSizeFraction = 1.0f;//thumb 的大小所占的比例
		m_minThumbSize = 20.0f;
		m_orientation = arguments.morientation;

		m_childrens.push_back(arguments.mTopSlot);
		m_childrens.push_back(arguments.mBottomSlot);
		m_childrens.push_back(arguments.mThumbSlot);

		m_bIsAlwaysVisible = true;

		for (size_t i = 0; i < m_childrens.size(); ++i)
		{
			m_childrens[i]->m_parentWidget = shared_from_this();
			m_childrens[i]->m_childWidget->setParentWidget(shared_from_this());
		}
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
	void ScrollBarTrack::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		const float width = allocatedGeometry.mLocalSize.x;
		const float height = allocatedGeometry.mLocalSize.y;

		//我们只需要展示所有三个儿子 ，当thumb可见的时候 ，否则我们只需要去显示track
		if (isNeeded())
		{
			TrackSizeInfo trackSizeInfo = getTrackSizeInfo(allocatedGeometry);

			//安排 track 的上半部分
			math::float2 childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbStart, height)
				: math::float2(width, trackSizeInfo.m_thumbStart);

			math::float2 childPos(0, 0);
			WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[TOP_SLOT_INDEX]->getChildWidget());

			//安排 track 的下半部分
			childPos = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.getThumbEnd(), 0)
				: math::float2(0, trackSizeInfo.getThumbEnd());

			childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(allocatedGeometry.getLocalSize().x - trackSizeInfo.getThumbEnd(), height)
				: math::float2(width, allocatedGeometry.getLocalSize().y - trackSizeInfo.getThumbEnd());

			childGeometry = allocatedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[BOTTOM_SLOT_INDEX]->getChildWidget());

			//安排 thumb
			childPos = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbStart, 0)
				: math::float2(0, trackSizeInfo.m_thumbStart);

			childSize = (m_orientation == Orientation::Horizontal)
				? math::float2(trackSizeInfo.m_thumbSize, height)
				: math::float2(width, trackSizeInfo.m_thumbSize);

			childGeometry = allocatedGeometry.getChildGeometry(childSize, childPos);
			arrangedWidgetArray.pushWidget(childGeometry, m_childrens[THUMB_SLOT_INDEX]->getChildWidget());
		}
		else
		{
			WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(width, height), math::float2(0, 0));
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
	SlotBase* ScrollBarTrack::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}
	uint32_t ScrollBarTrack::getSlotsNumber() const
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
	float ScrollBarTrack::getMinThumbSize() const
	{
		return m_minThumbSize;
	}
	float ScrollBarTrack::distanceFromTop() const
	{
		return m_offsetFraction;
	}
	float ScrollBarTrack::distanceFromBottom() const
	{
		return 1.0f - (m_offsetFraction + m_thumbSizeFraction);
	}
	float ScrollBarTrack::getThumbSizeFraction() const
	{
		return m_thumbSizeFraction;
	}
	void ScrollBarTrack::setSizes(float inThumbOffsetFraction, float inThumbSizeFraction)
	{
		m_offsetFraction = inThumbOffsetFraction;
		m_thumbSizeFraction = inThumbSizeFraction;

		if (m_thumbSizeFraction == 0.0f && !m_bIsAlwaysVisible)
		{
			m_thumbSizeFraction = 1.0f;
		}
		else if (m_thumbSizeFraction > 1.0f && m_bIsAlwaysVisible)
		{
			m_thumbSizeFraction = 0.0f;
		}
	}
}