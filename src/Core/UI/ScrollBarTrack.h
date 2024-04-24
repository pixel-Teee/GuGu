#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	class ScrollBarTrack : public Widget
	{
	public:
		ScrollBarTrack();

		virtual ~ScrollBarTrack();

		class ScrollBarTrackSlot : public Slot<ScrollBarTrack>
		{
		public:
			ScrollBarTrackSlot(){}

			virtual ~ScrollBarTrackSlot() = default;
		};

		struct BuilderArguments : public Arguments<ScrollBarTrack>
		{
			BuilderArguments()
				: morientation(Orientation::Vertical)
			{
				//mVisibility = Visibility::SelfHitTestInvisible;
				//mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(ScrollBarTrackSlot, TopSlot)
			ARGUMENT_NAMED_SLOT(ScrollBarTrackSlot, ThumbSlot)
			ARGUMENT_NAMED_SLOT(ScrollBarTrackSlot, BottomSlot)
			ARGUMENT_VALUE(Orientation, orientation)
		};

		struct TrackSizeInfo
		{
			TrackSizeInfo(const WidgetGeometry& trackGeometry, Orientation inOrientation, float inMinThumbSize, float thumbSizeAsFractionOfTrack, float thumbOffsetAsFractionOfTrack)
			{
				//thumb size as fraction of track 是 track 的百分比
				m_biasedTrackSize = ((inOrientation == Orientation::Horizontal) ? trackGeometry.getLocalSize().x : trackGeometry.getLocalSize().y) - inMinThumbSize;
				const float accurateThumbSize = thumbSizeAsFractionOfTrack * m_biasedTrackSize;
				m_thumbStart = m_biasedTrackSize * thumbOffsetAsFractionOfTrack;
				m_thumbSize = inMinThumbSize + accurateThumbSize;
			}
			float m_biasedTrackSize;//去掉 thumb 的轨道大小
			float m_thumbStart;//track 的上半部分宽度或者高度
			float m_thumbSize;//thumb 大小
			float getThumbEnd()
			{
				return m_thumbStart + m_thumbSize;
			}
		};

		void init(const BuilderArguments& arguments);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		bool isNeeded() const;

		TrackSizeInfo getTrackSizeInfo(const WidgetGeometry& inTrackGeometry) const;

		float getMinThumbSize() const;
	protected:

		std::vector<std::shared_ptr<ScrollBarTrackSlot>> m_childrens;

		static const int32_t TOP_SLOT_INDEX = 0;
		static const int32_t BOTTOM_SLOT_INDEX = 1;
		static const int32_t THUMB_SLOT_INDEX = 2;

		float m_offsetFraction; //thumb 移动的偏移
		float m_thumbSizeFraction;//thumb 占 track 的百分比
		float m_minThumbSize;
		bool m_bIsAlwaysVisible = false;
		Orientation m_orientation;
	};
}