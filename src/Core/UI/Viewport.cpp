#include <pch.h>

#include "Viewport.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"
#include "ElementList.h"

namespace GuGu {
	ViewportWidget::ViewportWidget()
	{
	}
	ViewportWidget::~ViewportWidget()
	{
	}
	void ViewportWidget::init(const BuilderArguments& arguments)
	{
		m_childWidget = arguments.mContent;
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
		m_widgetClipping = arguments.mClip;
	}
	uint32_t ViewportWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		if (m_renderTarget)
		{
			ElementList::addViewportElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_renderTarget, layer + 1);
		}
		
		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return layer + 1;
	}
	math::float2 ViewportWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2(600.0f, 480.0f);//todo:fix this
	}
	void ViewportWidget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		uint32_t slotNumber = getSlotsNumber();
		if (slotNumber)
		{
			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset), allocatedGeometry.getAccumulateTransform());

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}		
		}
	}
	SlotBase* ViewportWidget::getSlot(uint32_t index)
	{
		return m_childWidget.get();
	}
	uint32_t ViewportWidget::getSlotsNumber()
	{
		return 1;
	}
	void ViewportWidget::setRenderTarget(nvrhi::TextureHandle renderTarget)
	{
		m_renderTarget = renderTarget;
	}
}