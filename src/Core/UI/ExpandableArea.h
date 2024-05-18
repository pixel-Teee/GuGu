#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"
#include "Border.h"
#include "WidgetDelegates.h"

namespace GuGu {
	class ExpandableArea : public Widget
	{
	public:
		struct BuilderArguments : public Arguments<ExpandableArea>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ExpandableAreaStyle>, style)
				
			ARGUMENT_ATTRIBUTE(math::float4, borderBackgroundColor)
	
			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, borderImage)
			
			ARGUMENT_ATTRIBUTE(math::float4, bodyBorderBackgroundColor)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, bodyBorderImage)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, HeaderContent)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, BodyContent)

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, AreaTitle)

			ARGUMENT_VALUE(bool, initiallyCollapsed)

			ARGUMENT_VALUE(float, minWidth)

			ARGUMENT_VALUE(float, maxHeight)

			ARGUMENT_ATTRIBUTE(Padding, AreaTitlePadding)

			ARGUMENT_ATTRIBUTE(Padding, HeaderPadding)

			ARGUMENT_ATTRIBUTE(Padding, padding)

			UI_EVENT(OnBooleanValueChanged, onAreaExpansionChanged)
		};
		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	
		Reply onHeaderClicked();

		virtual void setExpandedAnimated(bool bExpanded);

		virtual void onToggleContentVisibility();

		virtual Visibility onGetContentVisibility() const;
	protected:
		std::shared_ptr<SingleChildSlot> m_childWidget;

		std::shared_ptr<Border> m_titleBorder;

		OnBooleanValueChanged m_onAreaExpansionChanged;

		float m_minWidth;

		float m_maxHeight;

		bool m_bAreaCollapsed;

		std::shared_ptr<Brush> m_collapsedImage;

		std::shared_ptr<Brush> m_expandedImage;
	};
}