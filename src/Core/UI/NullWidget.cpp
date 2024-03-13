#include <pch.h>

#include "NullWidget.h"

namespace GuGu {
	NullWidget::NullWidget()
	{
	}
	NullWidget::~NullWidget()
	{
	}
	void NullWidget::init(const BuilderArguments& arguments)
	{
	}
	uint32_t NullWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		return layer;
	}
	math::float2 NullWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2(0.0f, 0.0f);
	}
	SlotBase* NullWidget::getSlot(uint32_t index)
	{
		return nullptr;
	}
	uint32_t NullWidget::getSlotsNumber()
	{
		return 0;
	}
}