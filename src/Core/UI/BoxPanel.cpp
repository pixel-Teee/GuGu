#include <pch.h>

#include "BoxPanel.h"

namespace GuGu {
	BoxPanel::BoxPanel()
		: m_orientation(Orientation::Horizontal)
	{
	}
	BoxPanel::~BoxPanel()
	{
	}
	void BoxPanel::init(const BuilderArguments& arguments)
	{
	}
	uint32_t BoxPanel::GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		return layer;
	}
	math::double2 BoxPanel::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::double2(0.0, 0.0);
	}
	void BoxPanel::AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		
	}
}