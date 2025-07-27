#include <pch.h>

#include "UniformGridPanel.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"

namespace GuGu {

	UniformGridPanel::UniformGridPanel()
	{

	}

	UniformGridPanel::~UniformGridPanel()
	{

	}

	void UniformGridPanel::init(const BuilderArguments& arguments)
	{
		m_slotPadding = arguments.mslotPadding;
		m_numColumns = 0;
		m_numRows = 0;
		m_minDesiredSlotWidth = arguments.mminDesiredSlotWidth;
		m_minDesiredSlotHeight = arguments.mminDesiredSlotHeight;

		//add slots
		m_childrens.resize(arguments.mSlots.size());
		for (int32_t slotIndex = 0; slotIndex < arguments.mSlots.size(); ++slotIndex)
		{
			std::shared_ptr<UniformGridPanel::UniformGridSlot> uniformSlot = std::static_pointer_cast<UniformGridPanel::UniformGridSlot>(arguments.mSlots[slotIndex].m_slot);
			UniformGridSlot::SlotBuilderArguments builderArguments = arguments.mSlots[slotIndex];
			uniformSlot->init(shared_from_this(), builderArguments);
			m_childrens[slotIndex] = uniformSlot;
		}
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}

	UniformGridPanel::UniformGridSlot::SlotBuilderArguments UniformGridPanel::Slot(int32_t column, int32_t row)
	{
		 return 
		 UniformGridPanel::UniformGridSlot::SlotBuilderArguments(std::make_shared<UniformGridSlot>())
		 .Column(column)
	     .Row(row);
	}

	uint32_t UniformGridPanel::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		uint32_t maxLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer);
				maxLayerId = std::max(maxLayerId, curWidgetsMaxLayerId);
			}
		}

		return maxLayerId;
	}

	math::float2 UniformGridPanel::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 maxChildFixedSize = math::float2(0, 0);
		const math::float2 slotPaddingFixedSize = m_slotPadding.Get().getFixedSize();

		const float cachedMinDesiredSlotWidth = m_minDesiredSlotWidth.Get();
		const float cachedMinDesiredSlotHeight = m_minDesiredSlotHeight.Get();

		m_numColumns = 0;
		m_numRows = 0;

		for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
		{
			const std::shared_ptr<UniformGridSlot>& child = m_childrens[childIndex];
			if (child->getChildWidget()->getVisibility() != Visibility::Collapsed)
			{
				m_numColumns = std::max(child->m_column + 1, m_numColumns);
				m_numRows = std::max(child->m_row + 1, m_numRows);

				math::float2 childFixedSize = child->getChildWidget()->getFixedSize() + slotPaddingFixedSize;

				childFixedSize.x = std::max(childFixedSize.x, cachedMinDesiredSlotWidth);
				childFixedSize.y = std::max(childFixedSize.y, cachedMinDesiredSlotHeight);

				maxChildFixedSize.x = std::max(maxChildFixedSize.x, childFixedSize.x);
				maxChildFixedSize.y = std::max(maxChildFixedSize.y, childFixedSize.y);
			}
		}

		return math::float2(m_numColumns * maxChildFixedSize.x, m_numRows * maxChildFixedSize.y);
	}

	void UniformGridPanel::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		if (m_childrens.size() > 0)
		{
			const math::float2 cellSize(allocatedGeometry.getLocalSize().x / m_numColumns, allocatedGeometry.getLocalSize().y / m_numRows);
			const Padding& currentSlotPadding(m_slotPadding.Get());
			for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
			{
				const std::shared_ptr<UniformGridSlot>& child = m_childrens[childIndex];

				const Visibility childVisiblity = child->getChildWidget()->getVisibility();

				if (arrangedWidgetArray.accepts(childVisiblity))
				{
					AlignmentArrangeResult xAlignmentResult = AlignChild<Orientation::Horizontal>(*child, cellSize.x, currentSlotPadding);
					AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*child, cellSize.y, currentSlotPadding);

					WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(
						math::float2(xAlignmentResult.m_size, yAlignmentResult.m_size),
						math::float2(cellSize.x * child->m_column + xAlignmentResult.m_offset, cellSize.y * child->m_row + yAlignmentResult.m_offset));

					arrangedWidgetArray.pushWidget(childGeometry, child->getChildWidget());
				}
			}
		}
	}

	SlotBase* UniformGridPanel::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}

	uint32_t UniformGridPanel::getSlotsNumber() const
	{
		return m_childrens.size();
	}

	UniformGridPanel::ScopedWidgetSlotArguments<UniformGridPanel::UniformGridSlot> UniformGridPanel::addSlot(int32_t column, int32_t row)
	{
		return
		UniformGridPanel::ScopedWidgetSlotArguments<UniformGridPanel::UniformGridSlot>
			(std::make_shared<UniformGridSlot>(column, row), 
			m_childrens, 
			-1, 
			std::static_pointer_cast<UniformGridPanel>(shared_from_this()));
	}

}