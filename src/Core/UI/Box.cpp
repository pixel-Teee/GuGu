#include <pch.h>

#include "box.h"

#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"

namespace GuGu {
	BoxWidget::BoxWidget()
	{
	}
	BoxWidget::~BoxWidget()
	{
	}
	void BoxWidget::init(const BuilderArguments& arguments)
	{
		m_widthOverride = arguments.mWidthOverride;
		m_heightOverride = arguments.mHeightOverride;

		m_minDesiredWidth = arguments.mMinDesiredWidth;
		m_minDesiredHeight = arguments.mMinDesiredHeight;
		m_maxDesiredWidth = arguments.mMaxDesiredWidth;
		m_maxDesiredHeight = arguments.mMaxDesiredHeight;

		m_minAspectRatio = arguments.mMinAspectRatio;
		m_maxAspectRatio = arguments.mMaxAspectRatio;

		m_childWidget = arguments.mContent;
		m_childWidget->setVerticalAlignment(arguments.mVAlign);
		m_childWidget->setHorizontalAlignment(arguments.mHAlign);
		m_childWidget->setPadding(arguments.mpadding.Get());
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
	}

	math::float2 BoxWidget::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				const OptionalSize currentWidthOverride = m_widthOverride.Get();
				const OptionalSize currentHeightOverride = m_heightOverride.Get();

				return math::float2((currentWidthOverride.isSet() ? currentWidthOverride.get() : computeDesiredWidth()), (currentHeightOverride.isSet() ? currentHeightOverride.get() : computeDesiredHeight()));
			}
		}
		return math::float2(0, 0);
	}
	uint32_t BoxWidget::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		//if (m_windowType != WindowType::NativeWindow)
		//{
		//	ElementList::addBoxElement(elementList, allocatedGeometry, math::float4(1.0f, 1.0f, 1.0f, 1.0f), m_defaultBrush, layer); //background
		//}

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

		return maxLayer;
	}
	void BoxWidget::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		const Visibility childVisibility = m_childWidget->getChildWidget()->getVisibility();

		if (arrangedWidgetArray.accepts(childVisibility))
		{
			const OptionalSize currentMinAspectRatio = m_minAspectRatio.Get();
			const OptionalSize currentMaxAspectRatio = m_maxAspectRatio.Get();

			const Padding slotPadding(m_childWidget->getPadding());

			bool bAlignChildren = true;

			AlignmentArrangeResult xAlignmentResult(0, 0);
			AlignmentArrangeResult yAlignmentResult(0, 0);

			if (currentMinAspectRatio.isSet() || currentMaxAspectRatio.isSet())
			{
				//...
			}

			if (bAlignChildren)
			{
				Padding slotPadding = m_childWidget->getPadding();
				xAlignmentResult = AlignChild<Orientation::Horizontal>(*m_childWidget, allocatedGeometry.getLocalSize().x, slotPadding);
				yAlignmentResult = AlignChild<Orientation::Vertical>(*m_childWidget, allocatedGeometry.getLocalSize().y, slotPadding);
			}

			const float alignedSizeX = xAlignmentResult.m_size;
			const float alignedSizeY = yAlignmentResult.m_size;

			WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xAlignmentResult.m_size, yAlignmentResult.m_size), math::float2(xAlignmentResult.m_offset, yAlignmentResult.m_offset));

			arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
		}
	}
	SlotBase* BoxWidget::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t BoxWidget::getSlotsNumber() const
	{
		return 1;
	}
	float BoxWidget::computeDesiredWidth() const
	{
		const math::float2& unmodifiedChildDesiredSize = m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();

		const OptionalSize currentMinDesiredWidth = m_minDesiredWidth.Get();
		const OptionalSize currentMaxDesiredWidth = m_maxDesiredWidth.Get();

		float currentWidth = unmodifiedChildDesiredSize.x;

		if (currentMinDesiredWidth.isSet())
		{
			currentWidth = std::max(currentWidth, currentMinDesiredWidth.get());
		}

		if (currentMaxDesiredWidth.isSet())
		{
			currentWidth = std::min(currentWidth, currentMaxDesiredWidth.get());
		}

		return currentWidth;
	}
	float BoxWidget::computeDesiredHeight() const
	{
		const math::float2& unmodifiedChildDesiredSize = m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();

		const OptionalSize currentMinDesiredHeight = m_minDesiredHeight.Get();
		const OptionalSize currentMaxDesiredHeight = m_maxDesiredHeight.Get();

		float currentHeight = unmodifiedChildDesiredSize.y;

		if (currentMinDesiredHeight.isSet())
		{
			currentHeight = std::max(currentHeight, currentMinDesiredHeight.get());
		}

		if (currentMaxDesiredHeight.isSet())
		{
			currentHeight = std::min(currentHeight, currentMaxDesiredHeight.get());
		}

		return currentHeight;
	}
}