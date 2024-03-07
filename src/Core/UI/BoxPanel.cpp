#include <pch.h>

#include "BoxPanel.h"

#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	template<Orientation orientation, typename SlotType>
	static void ArrangeChildrenAlong(std::vector<SlotType>& childrens, WidgetGeometry& widgetGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		if (childrens.size() > 0)
		{
			float stretchCoefficientTotal = 0;
			float fixedTotal = 0;
		
			for (uint32_t childIndex = 0; childIndex < childrens.size(); ++childIndex)
			{
				const SlotType& curChild = childrens[childIndex];

				fixedTotal += curChild->getPadding().template getTotalSpaceAlong<orientation>();

				if (curChild->getSizeRule() == SizeParam::Stretch)
				{
					//for stretch children we save sum up the stretch coefficients
					stretchCoefficientTotal += curChild->getSizeValue();
				}
				else
				{
					math::double2 childFixedSize = curChild->getChildWidget()->getFixedSize();

					//fixed size children contribute their fixed size to the fixed space requirement
					const float childSize = (orientation == Orientation::Vertical) ? childFixedSize.y : childFixedSize.x;

					//clamp to the max size if it was specified
					float maxSize = curChild->getMaxSize();
					fixedTotal += maxSize > 0 ? std::min(maxSize, childSize) : childSize;
				}
			}

			//the space available for size rule stretch widgets is any space that wasn't taken up by fixed-sized widgets
			const float nonFixedSpace = std::max(0.0f, (orientation == Orientation::Vertical) 
				? (float)widgetGeometry.getLocalSize().y - fixedTotal
				: (float)widgetGeometry.getLocalSize().x - fixedTotal);

			float positionSoFar = 0;

			for (size_t i = 0; i < childrens.size(); ++i)
			{
				const SlotType& curChild = childrens[i];

				float childSize = 0;
				if (curChild->getSizeRule() == SizeParam::Stretch)
				{
					if (stretchCoefficientTotal > 0)
						childSize = nonFixedSpace * curChild->getSizeValue() / stretchCoefficientTotal;
				}
				else
				{
					math::double2 childFixedSize = curChild->getChildWidget()->getFixedSize();

					//fixed sized widgets get their desired size value
					childSize = (orientation == Orientation::Vertical) ? childFixedSize.y : childFixedSize.x;
				}

				//clamp to the max size if it is was specified
				float maxSize = curChild->getMaxSize();
				if (maxSize)
				{
					childSize = std::min(maxSize, childSize);
				}

				const Padding slotPadding(curChild->getPadding());

				math::float2 slotSize = (orientation == Orientation::Vertical)
					? math::float2(widgetGeometry.getLocalSize().x, childSize + curChild->getPadding().template getTotalSpaceAlong<orientation>())
					: math::float2(childSize + curChild->getPadding().template getTotalSpaceAlong<orientation>(), widgetGeometry.getLocalSize().y);

				AlignmentArrangeResult xAlignmentResult = AlignChild<Orientation::Horizontal>(*curChild, slotSize.x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*curChild, slotSize.y);

				const math::float2 localPosition = (orientation == Orientation::Vertical)
					? math::float2(xAlignmentResult.m_offset, positionSoFar + yAlignmentResult.m_offset)
					: math::float2(positionSoFar + xAlignmentResult.m_offset, yAlignmentResult.m_offset);

				const math::float2 localSize = math::float2(xAlignmentResult.m_size, yAlignmentResult.m_size);

				arrangedWidgetArray.pushWidget(widgetGeometry.getChildGeometry(math::double2(localSize.x, localSize.y), math::double2(localPosition.x, localPosition.y), widgetGeometry.getAccumulateTransform()), curChild->getChildWidget());

				//offset the next child by the size of the current child and any post-child (bottom/right) margin
				positionSoFar += (orientation == Orientation::Vertical) ? slotSize.y : slotSize.x;
			}
		}
	}

	template<Orientation orientation, typename SlotType>
	static math::float2 ComputeDesiredSizeForBox(std::vector<SlotType>& childrens)
	{
		math::float2 myDesiredSize(0, 0);

		for (uint32_t childIndex = 0; childIndex < childrens.size(); ++childIndex)
		{
			const SlotType& curChild = childrens[childIndex];

			math::double2 childFixedSize = curChild->getChildWidget()->getFixedSize();

			if (orientation == Orientation::Vertical)
			{
				myDesiredSize.x = std::max(myDesiredSize.x, (float)childFixedSize.x + curChild->getPadding().template getTotalSpaceAlong<Orientation::Horizontal>());

				//clamp to the max size if it was specified
				float finalChildDesiredSize = childFixedSize.y;
				float maxSize = curChild->getMaxSize();
				if (maxSize)
				{
					finalChildDesiredSize = std::min(maxSize, finalChildDesiredSize);
				}

				myDesiredSize.y += finalChildDesiredSize + curChild->getPadding().template getTotalSpaceAlong<Orientation::Vertical>();
			}
			else
			{
				myDesiredSize.y = std::max(myDesiredSize.y, (float)childFixedSize.y + curChild->getPadding().template getTotalSpaceAlong<Orientation::Vertical>());

				//clamp to the max size if it was specified
				float finalChildDesiredSize = childFixedSize.x;
				float maxSize = curChild->getMaxSize();
				if (maxSize)
				{
					finalChildDesiredSize = std::min(maxSize, finalChildDesiredSize);
				}

				myDesiredSize.x += finalChildDesiredSize + curChild->getPadding().template getTotalSpaceAlong<Orientation::Horizontal>();
			}
		}

		return myDesiredSize;
	}
	BoxPanel::BoxPanel()
		: m_orientation(Orientation::Horizontal)
	{
	}
	BoxPanel::BoxPanel(Orientation orientation)
		: m_orientation(orientation)
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
		uint32_t maxLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray;
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->GenerateElement(elementList, childWidget->getWidgetGeometry(), layer);
				maxLayerId = std::max(maxLayerId, curWidgetsMaxLayerId);
			}		
		}

		return maxLayerId;
	}
	math::double2 BoxPanel::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 fixedSize = (m_orientation == Orientation::Horizontal) 
			? ComputeDesiredSizeForBox<Orientation::Horizontal>(m_childrens)
			: ComputeDesiredSizeForBox<Orientation::Vertical>(m_childrens);

		return math::double2(fixedSize.x, fixedSize.y);
	}
	void BoxPanel::AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		if (m_orientation == Orientation::Horizontal)
		{
			ArrangeChildrenAlong<Orientation::Horizontal>(m_childrens, allocatedGeometry, arrangedWidgetArray);
		}
		else
		{
			ArrangeChildrenAlong<Orientation::Vertical>(m_childrens, allocatedGeometry, arrangedWidgetArray);
		}
	}

	SlotBase* BoxPanel::getSlot(uint32_t index)
	{
		return m_childrens[index].get();
	}

	uint32_t BoxPanel::getSlotsNumber()
	{
		return m_childrens.size();
	}

	HorizontalBox::HorizontalBox()
		: BoxPanel(Orientation::Horizontal)
	{
	}
	HorizontalBox::~HorizontalBox()
	{
	}
	void HorizontalBox::init(const BuilderArguments& arguments)
	{
		m_childrens.resize(arguments.mSlots.size());
		for (size_t i = 0; i < arguments.mSlots.size(); ++i)
		{
			std::shared_ptr<HorizontalBox::HorizontalBoxSlot> boxSlot = std::static_pointer_cast<HorizontalBox::HorizontalBoxSlot>(arguments.mSlots[i].m_slot);
			HorizontalBoxSlot::SlotBuilderArguments builderArguments = arguments.mSlots[i];
			boxSlot->init(builderArguments);
			m_childrens[i] = std::reinterpret_pointer_cast<BoxPanelSlot>(boxSlot);
		}
	}
}