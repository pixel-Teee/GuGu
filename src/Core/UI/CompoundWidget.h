#pragma once

#include "Slot.h"
#include "Widget.h"

namespace GuGu {
	class CompoundWidget : public Widget
	{
	public:
		CompoundWidget();

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	protected:
		std::shared_ptr<SingleChildSlot> m_childWidget; //single child slot

		//用于动画的缩放值
		math::float2 m_contentScale;

		math::float4 m_colorAndOpacity;
	};
}