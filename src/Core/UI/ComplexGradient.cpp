#include <pch.h>

#include "ComplexGradient.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "ElementList.h"

namespace GuGu {
	ComplexGradient::ComplexGradient()
	{
	}
	ComplexGradient::~ComplexGradient()
	{
	}
	void ComplexGradient::init(const BuilderArguments& arguments)
	{
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;

		m_desiredSizeOverried = arguments.mdesiredSizeOverride;
		m_gradientColors = arguments.mGradientColors;
		m_bHasAlphaBackground = arguments.mhasAlphaBackground;
		m_orientation = arguments.morientation;
		m_cornerRadius = arguments.mcornerRadius;
	}
	uint32_t ComplexGradient::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		//todo:add alpha background

		const std::vector<math::float4>& colors = m_gradientColors;
		int32_t numColors = colors.size();

		if (numColors)
		{
			std::vector<GradientStop> gradientStop;

			for (int32_t colorIndex = 0; colorIndex < numColors; ++colorIndex)
			{
				gradientStop.push_back(GradientStop(allocatedGeometry.getLocalSize() * (float(colorIndex) / (numColors - 1)), colors[colorIndex]));
			}

			ElementList::addGradientElement(
				elementList, 
				allocatedGeometry, 
				gradientStop, 
				m_orientation,
				m_cornerRadius, layer + 1);
		}

		return layer + 1;
	}
	math::float2 ComplexGradient::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_desiredSizeOverried.has_value())
		{
			return m_desiredSizeOverried.value();
		}
		return Widget::ComputeFixedSize(inLayoutScaleMultiplier);
	}
	void ComplexGradient::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();

		if (slotNumber)
		{
			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}
		}
	}
	SlotBase* ComplexGradient::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t ComplexGradient::getSlotsNumber() const
	{
		if (m_childWidget) return 1;
		return 0;
	}
}