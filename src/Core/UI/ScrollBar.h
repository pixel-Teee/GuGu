#pragma once

#include "Border.h"
#include "StyleSet.h"

namespace GuGu {
	using OnUserScrolled = std::function<void(float)>;//scroll offset 在 0 和 1 之间

	class ImageWidget;
	class ScrollBarTrack;
	class Spacer;
	class ScrollBar : public Border
	{
	public:
		ScrollBar();

		virtual ~ScrollBar();

		struct BuilderArguments : public Arguments<ScrollBar>
		{
			BuilderArguments()
				: mstyle(StyleSet::getStyle()->template getStyle<ScrollBarStyle>("ScrollBar"))
				, mthickNess()
				, morientation(Orientation::Vertical)
				, mpadding(Padding(2.0f))
				, monUserScrolled()
			{}

			~BuilderArguments() = default;
			
			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)
			//scroll bar thumb 的粗细度
			ARGUMENT_ATTRIBUTE(math::float2, thickNess)

			ARGUMENT_ATTRIBUTE(Padding, padding)

			ARGUMENT_VALUE(Orientation, orientation)

			UI_EVENT(OnUserScrolled, onUserScrolled)
		};

		void init(const BuilderArguments& arguments);

		void setStyle(const std::shared_ptr<ScrollBarStyle> inStyle);

		Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		//执行用户的 scrolled 委托
		void executeOnUserScrolled(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		void setOnUserScrolled(const OnUserScrolled& inHandler);

		void setState(float inOffsetFraction, float inThumbSizeFraction);

		float distanceFromBottom() const;
	protected:
		std::shared_ptr<ImageWidget> m_topImage;
		std::shared_ptr<ImageWidget> m_bottomImage;
		std::shared_ptr<Border> m_dragThumb;
		std::shared_ptr<Spacer> m_thicknessSpacer;
		std::shared_ptr<ScrollBarTrack> m_track;
		OnUserScrolled m_onUserScrolled;
		Orientation m_orientation;

		std::shared_ptr<Brush> m_normalThumbImage;
		std::shared_ptr<Brush> m_backgroundBrush;
		std::shared_ptr<Brush> m_topBrush;
		std::shared_ptr<Brush> m_bottomBrush;
		bool m_bDraggingThumb;
		float m_dragGrabOffset;
	};
}