#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	class ComplexGradient : public Widget
	{
	public:
		ComplexGradient();

		virtual ~ComplexGradient();

		struct BuilderArguments : public Arguments<ComplexGradient>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(std::vector<math::float4>, GradientColors)

			ARGUMENT_VALUE(bool, hasAlphaBackground)

			ARGUMENT_VALUE(Orientation, orientation)

			ARGUMENT_VALUE(std::optional<math::float2>, desiredSizeOverride)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_VALUE(math::float4, cornerRadius)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	protected:

		std::vector<math::float4> m_gradientColors;

		std::optional<math::float2> m_desiredSizeOverried;

		bool m_bHasAlphaBackground;

		Orientation m_orientation;

		std::shared_ptr<SingleChildSlot> m_childWidget;

		math::float4 m_cornerRadius;
	};
}