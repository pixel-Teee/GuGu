#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	class BoxWidget : public Widget
	{
	public:
		BoxWidget();

		virtual ~BoxWidget();

		struct BuilderArguments : public Arguments<BoxWidget>
		{
			BuilderArguments()
				: mHAlign(HorizontalAlignment::Stretch)
				, mVAlign(VerticalAlignment::Stretch)
				, mpadding(Attribute<Padding>(Padding(0, 0, 0, 0)))
				, mWidthOverride(OptionalSize())
				, mHeightOverride(OptionalSize())
				, mMinDesiredWidth(OptionalSize())
				, mMinDesiredHeight(OptionalSize())
				, mMaxDesiredWidth(OptionalSize())
				, mMaxDesiredHeight(OptionalSize())
			{}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(HorizontalAlignment, HAlign)

			ARGUMENT_VALUE(VerticalAlignment, VAlign)

			ARGUMENT_ATTRIBUTE(Padding, padding)

			ARGUMENT_ATTRIBUTE(OptionalSize, WidthOverride)

			ARGUMENT_ATTRIBUTE(OptionalSize, HeightOverride)

			ARGUMENT_ATTRIBUTE(OptionalSize, MinDesiredWidth)

			ARGUMENT_ATTRIBUTE(OptionalSize, MinDesiredHeight)

			ARGUMENT_ATTRIBUTE(OptionalSize, MaxDesiredWidth)

			ARGUMENT_ATTRIBUTE(OptionalSize, MaxDesiredHeight)

			ARGUMENT_ATTRIBUTE(OptionalSize, MinAspectRatio)

			ARGUMENT_ATTRIBUTE(OptionalSize, MaxAspectRatio)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index)  const override;

		virtual uint32_t getSlotsNumber() const override;
		//static Slot::SlotBuilderArguments SingleSlot() { return Slot::SlotBuilderArguments(std::make_shared<Slot>()); };

		float computeDesiredWidth() const;

		float computeDesiredHeight() const;
	private:
		//当描述，忽略content的固定大小，还有将 width override 作为 box 的固定大小
		Attribute<OptionalSize> m_widthOverride;

		Attribute<OptionalSize> m_heightOverride;

		Attribute<OptionalSize> m_minDesiredWidth;

		Attribute<OptionalSize> m_minDesiredHeight;

		Attribute<OptionalSize> m_maxDesiredWidth;

		Attribute<OptionalSize> m_maxDesiredHeight;

		Attribute<OptionalSize> m_minAspectRatio;

		Attribute<OptionalSize> m_maxAspectRatio;

		std::shared_ptr<SingleChildSlot> m_childWidget;
	};
}