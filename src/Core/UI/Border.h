#pragma once

#include "Widget.h"
#include "Slot.h"
#include "Brush.h"
#include "Attribute.h"
#include "UIMacros.h"
#include "CoreStyle.h"

namespace GuGu {
	class Border : public Widget
	{
	public:
		Border();

		virtual ~Border();

		struct BuilderArguments : public Arguments<Border>
		{
			BuilderArguments()
				: mbrush(CoreStyle::getStyleSet()->getBrush("white"))
				, mverticalAlignment(VerticalAlignment::Stretch)
				, mhorizontalAlignment(HorizontalAlignment::Stretch)
				, mpadding(Padding(0.0f, 0.0f, 0.0f, 0.0f))
				, mBorderBackgroundColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_VALUE(VerticalAlignment, verticalAlignment)
			
			ARGUMENT_VALUE(HorizontalAlignment, horizontalAlignment)
			
			ARGUMENT_ATTRIBUTE(Padding, padding)

			ARGUMENT_ATTRIBUTE(math::float4, BorderBackgroundColor)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		//设置 border image 属性
		void setBorderImage(Attribute<std::shared_ptr<Brush>> inBorderImage);

		void setContent(std::shared_ptr<Widget> inWidget);
	protected:
		Attribute<std::shared_ptr<Brush>> m_imageBursh;

		std::shared_ptr<SingleChildSlot> m_childWidget;

		Attribute<math::float4> m_borderBackgroundColor;
	};
}