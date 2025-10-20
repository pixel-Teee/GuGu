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
		m_visibilityAttribute = Visibility::SelfHitTestInvisible;

		m_desiredSizeOverried = arguments.mdesiredSizeOverride;
		m_gradientColors = arguments.mGradientColors;
		m_bHasAlphaBackground = arguments.mhasAlphaBackground;
		m_orientation = arguments.morientation;
		m_cornerRadius = arguments.mcornerRadius;
	}
	uint32_t ComplexGradient::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		uint32_t maxLayer = 0;
		//todo:add alpha background

		const std::vector<Attribute<math::float4>>& colors = m_gradientColors;
		int32_t numColors = colors.size();

		if (numColors)
		{
			std::vector<GradientStop> gradientStop;

			for (int32_t colorIndex = 0; colorIndex < numColors; ++colorIndex)
			{
				gradientStop.push_back(GradientStop(allocatedGeometry.getLocalSize() * (float(colorIndex) / (numColors - 1)), colors[colorIndex].Get()));
			}

			ElementList::addGradientElement(
				elementList, 
				allocatedGeometry, 
				gradientStop, 
				m_orientation,
				m_cornerRadius, layer + 1);
		}

		maxLayer = layer + 1;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}
		return maxLayer;
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
				Padding slotPadding = getSlot(0)->getPadding();
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x, slotPadding);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y, slotPadding);

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