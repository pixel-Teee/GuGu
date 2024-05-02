#include <pch.h>

#include "ScrollBar.h"
#include "box.h"
#include "BoxPanel.h"
#include "ScrollBarTrack.h"
#include "ImageWidget.h"
#include "Spacer.h"

namespace GuGu {
	ScrollBar::ScrollBar()
		: m_bDraggingThumb(false)
		, m_dragGrabOffset(0.0f)
	{
	}
	ScrollBar::~ScrollBar()
	{
	}
	void ScrollBar::init(const BuilderArguments& arguments)
	{
		m_orientation = arguments.morientation;
		m_onUserScrolled = arguments.monUserScrolled;//滑动的回调
		setStyle(arguments.mstyle.Get());

		const Attribute<math::float2> thickNess = arguments.mthickNess.IsSet() ? arguments.mthickNess : math::float2(arguments.mstyle.Get()->m_thickNess, arguments.mstyle.Get()->m_thickNess);

		HorizontalAlignment horizontalAlignment = m_orientation == Orientation::Vertical ? HorizontalAlignment::Center : HorizontalAlignment::Stretch;
		VerticalAlignment verticalAlignment = m_orientation == Orientation::Vertical ? VerticalAlignment::Stretch : VerticalAlignment::Center;

		Border::init(Border::BuilderArguments()
			.padding(arguments.mpadding.Get())
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.StretchHeight(1.0f)
				(
					WIDGET_NEW(Border)
					.brush(m_backgroundBrush)
					.horizontalAlignment(horizontalAlignment) //center
					.verticalAlignment(verticalAlignment) //fill
					.padding(0.0f)
					.Content
					(
						WIDGET_ASSIGN_NEW(ScrollBarTrack, m_track) //scroll bar track
						.orientation(arguments.morientation) //vertical
						.TopSlot //top part
						(
							WIDGET_NEW(BoxWidget)
							.HAlign(horizontalAlignment)
							.VAlign(verticalAlignment)
							.Content
							(
								WIDGET_ASSIGN_NEW(ImageWidget, m_topImage)
								.brush(m_topBrush)
							)
						)
						.ThumbSlot //thumb part
						(
							WIDGET_ASSIGN_NEW(Border, m_dragThumb)
							.horizontalAlignment(HorizontalAlignment::Center)
							.verticalAlignment(VerticalAlignment::Center)
							.padding(0.0f)
							.Content
							(
								WIDGET_ASSIGN_NEW(Spacer, m_thicknessSpacer) //占据一定空间的 thickness spacer
								.size(thickNess)
							)
							.brush(m_normalThumbImage)
						)
						.BottomSlot //bottom part
						(
							WIDGET_NEW(BoxWidget)
							.HAlign(horizontalAlignment) //center
							.VAlign(verticalAlignment) //fill
							.Content
							(
							
								WIDGET_ASSIGN_NEW(ImageWidget, m_bottomImage)
								.brush(m_bottomBrush)
							)
						)
					)
				)
			)
		);
	}
	void ScrollBar::setStyle(const std::shared_ptr<ScrollBarStyle> inStyle)
	{
		m_normalThumbImage = inStyle->m_normalThumbImage;

		if (m_orientation == Orientation::Vertical)
		{
			m_backgroundBrush = inStyle->m_verticalBackgroundImage;
			m_topBrush = inStyle->m_verticalTopSlotImage;
			m_bottomBrush = inStyle->m_verticalBottomSlotImage;
		}
		else
		{
			m_backgroundBrush = inStyle->m_horizontalBackgroundImage;
			m_topBrush = inStyle->m_horizontalTopSlotImage;
			m_bottomBrush = inStyle->m_horizontalBottomSlotImage;
		}
	}
	Reply ScrollBar::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		WidgetGeometry thumbGeometry = findChildGeometry(myGeometry, m_dragThumb);

		if (m_orientation == Orientation::Horizontal)
		{
			m_dragGrabOffset = thumbGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition).x;
		}
		else
		{
			m_dragGrabOffset = thumbGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition).y;
		}
		m_bDraggingThumb = true;

		if (m_bDraggingThumb)
		{
			return Reply::Handled().captureMouse(shared_from_this()).setFocus(shared_from_this());
		}

		return Reply::Unhandled();
	}
	Reply ScrollBar::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (this->hasMouseCapture())
		{
			if (!(math::isnear(inMouseEvent.getCursorDelta().x, 0) && math::isnear(inMouseEvent.getCursorDelta().y, 0)))
			{
				if (m_onUserScrolled)
				{
					executeOnUserScrolled(myGeometry, inMouseEvent);
				}				
				return Reply::Handled();
			}
		}
		return Reply::Unhandled();
	}
	Reply ScrollBar::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		m_bDraggingThumb = false;
		return Reply::Handled().releaseMouseCapture();
	}
	void ScrollBar::executeOnUserScrolled(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		const int32_t axisId = (m_orientation == Orientation::Horizontal) ? 0 : 1;
		const WidgetGeometry trackGeometry = findChildGeometry(myGeometry, m_track);//track bar 的 widget geometry
		const float unclampedOffsetInTrack = trackGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition)[axisId] - m_dragGrabOffset;
		const float trackSizeBiasedByMinThumbSize = trackGeometry.getLocalSize()[axisId] - m_track->getMinThumbSize();
		const float thumbOffsetInTrack = std::clamp(unclampedOffsetInTrack, 0.0f, std::max(trackSizeBiasedByMinThumbSize, 0.0f));
		const float thumbOffset = thumbOffsetInTrack / trackSizeBiasedByMinThumbSize;//thumb offset 处于[0, 1]之间
		if (m_onUserScrolled)
		{
			m_onUserScrolled(thumbOffset);
		}
	}
	void ScrollBar::setOnUserScrolled(const OnUserScrolled& inHandler)
	{
		m_onUserScrolled = inHandler;
	}
	void ScrollBar::setState(float inOffsetFraction, float inThumbSizeFraction)
	{
		if (m_track->distanceFromTop() != inOffsetFraction || m_track->getThumbSizeFraction() != inThumbSizeFraction)
		{
			m_track->setSizes(inOffsetFraction, inThumbSizeFraction);
		}
	}
	float ScrollBar::distanceFromBottom() const
	{
		return m_track->distanceFromBottom();
	}
}