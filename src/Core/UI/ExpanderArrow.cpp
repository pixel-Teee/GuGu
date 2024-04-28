#include <pch.h>

#include "ExpanderArrow.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"
#include "Button.h"
#include "ITableRow.h"
#include "StyleSet.h"
#include "ElementList.h"

namespace GuGu {
	ExpanderArrow::ExpanderArrow()
	{
	}
	ExpanderArrow::~ExpanderArrow()
	{
	}
	void ExpanderArrow::init(const BuilderArguments& arguments, const std::shared_ptr<ITableRow>& tableRow)
	{
		m_ownerRowPtr = tableRow;
		m_indentAmount = arguments.mindentAmount;
		m_baseIndentLevel = arguments.mbaseIndentLevel;
		m_shouldDrawWires = arguments.mshouldDrawWires;

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->setChildWidget(
			WIDGET_ASSIGN_NEW(Button, m_expanderArrow)
			.verticalAlignment(VerticalAlignment::Center)
			.horizontalAlignment(HorizontalAlignment::Center)
			.buttonSyle(StyleSet::getStyle()->getStyle<ButtonStyle>("NoBorder"))
		);
		m_childWidget->setPadding(Attribute<Padding>(this, &ExpanderArrow::getExpanderPadding));
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t ExpanderArrow::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		static const float wireThickness = 2.0f;
		static const float halfWireThickness = wireThickness / 2.0f;

		const float indent = m_indentAmount.Get(10.0f);
		const std::shared_ptr<Brush> verticalBarBrush = StyleSet::getStyle()->getNoBrush();

		if (m_shouldDrawWires.Get() == true && verticalBarBrush != nullptr)
		{
			const std::shared_ptr<ITableRow> ownerRow = m_ownerRowPtr.lock();
			math::float4 wireTint = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
			wireTint.w *= 0.15f;
			//绘制垂直的线表示路径到根节点
			const std::vector<int32_t>& needsWireByLevel = ownerRow->getWiresNeededByDepth();
			const int32_t numLevels = needsWireByLevel.size();

			for (int32_t level = 0; level < numLevels; ++level)
			{
				const float currentIndent = indent * level;

				if (needsWireByLevel[level])
				{
					ElementList::addBoxElement(elementList,
						allocatedGeometry.getChildGeometry(math::float2(wireThickness, allocatedGeometry.mLocalSize.y), math::float2(currentIndent - 3.0f, 0.0f)),
						wireTint,
						verticalBarBrush,
						layer
					);
				}
			}

			const float halfCellHeight = 0.5f * allocatedGeometry.mLocalSize.y;

			if (const bool bIsLastChild = ownerRow->isLastChild())
			{
				const float currentIndent = indent * (numLevels - 1);

				ElementList::addBoxElement(elementList,
					allocatedGeometry.getChildGeometry(math::float2(wireThickness, halfCellHeight + halfWireThickness), math::float2(currentIndent - 3.0f, 0.0f)),
					wireTint,
					verticalBarBrush,
					layer
				);
			}

			//note:
			if (const bool bItemAppearsExpanded = ownerRow->isItemExpanded() && ownerRow->doesItemHaveChildren())
			{
				const float currentIndent = indent * numLevels;
				//WidgetGeometry testGeometry = allocatedGeometry.getChildGeometry(
				//	math::float2(wireThickness, halfCellHeight + halfWireThickness),
				//	math::float2(currentIndent - 3.0f, halfCellHeight - halfWireThickness)
				//);
				ElementList::addBoxElement(elementList,
					allocatedGeometry.getChildGeometry(
						math::float2(wireThickness, halfCellHeight + halfWireThickness),
						math::float2(currentIndent - 3.0f, halfCellHeight - halfWireThickness)
					),
					wireTint,
					verticalBarBrush,
					layer
				);
			}

			//draw horizontal connector from parent wire to child
			{
				float leafDepth = ownerRow->doesItemHaveChildren() ? 10.0f : 0.0f;
				const float horizontalWireStart = (numLevels - 1) * indent;
				//WidgetGeometry testGeometry = allocatedGeometry.getChildGeometry(
				//	math::float2(allocatedGeometry.mLocalSize.x - horizontalWireStart - leafDepth, wireThickness),
				//	math::float2(horizontalWireStart + wireThickness - 3.0f, 0.5f * (allocatedGeometry.mLocalSize.y - wireThickness))
				//);
				ElementList::addBoxElement(elementList,
					allocatedGeometry.getChildGeometry(
						math::float2(allocatedGeometry.mLocalSize.x - horizontalWireStart - leafDepth, wireThickness),
						math::float2(horizontalWireStart + wireThickness - 3.0f, 0.5f * (allocatedGeometry.mLocalSize.y - wireThickness))
					),
					wireTint,
					verticalBarBrush,
					layer
				);
			}
		}

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

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
	math::float2 ExpanderArrow::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
			}
		}
		return math::float2(0, 0);
	}
	void ExpanderArrow::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one
		//assert(slotNumber == 1);

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
	SlotBase* ExpanderArrow::getSlot(uint32_t index) const
	{
		return m_childWidget.get();
	}
	uint32_t ExpanderArrow::getSlotsNumber() const
	{
		return 1;
	}
	Padding ExpanderArrow::getExpanderPadding() const
	{
		const int32_t nestingDepth = std::max(0, m_ownerRowPtr.lock()->getIndentLevel() - m_baseIndentLevel.Get());
		const float indent = m_indentAmount.Get(10.0f);

		return Padding(nestingDepth * indent, 0, 0, 0);
	}
}