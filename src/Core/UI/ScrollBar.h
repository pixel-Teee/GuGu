#pragma once

#include "Border.h"
#include "StyleSet.h"

namespace GuGu {
	class ImageWidget;
	class ScrollBarTrack;
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
			{}

			~BuilderArguments() = default;
			
			ARGUMENT_ATTRIBUTE(std::shared_ptr<ScrollBarStyle>, style)
			//scroll bar thumb 的粗细度
			ARGUMENT_ATTRIBUTE(math::float2, thickNess)

			ARGUMENT_VALUE(Orientation, orientation)
		};

		void init(const BuilderArguments& arguments);

	protected:
		std::shared_ptr<ImageWidget> m_topImage;
		std::shared_ptr<ImageWidget> m_bottomImage;
		std::shared_ptr<Border> m_dragThumb;
		std::shared_ptr<ScrollBarTrack> m_track;
		Orientation m_orientation;

	};
}