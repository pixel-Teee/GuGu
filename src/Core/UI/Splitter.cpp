#include <pch.h>

#include "Splitter.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	template<Orientation orientation>
	static math::float2 computeDesiredSizeForSplitter(const float physicalSplitterHandleSize, const std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children)
	{
		math::float2 myDesiredSize(0, 0);

		int32_t numNonCollapsed = 0;
		for (int32_t childIndex = 0; childIndex < children.size(); ++childIndex)
		{
			const Splitter::SplitterSlot& curSlot = *children[childIndex];
			const Visibility childVisibility = curSlot.getChildWidget()->getVisibility();
			if (childVisibility != Visibility::Collapsed)
			{
				++numNonCollapsed;

				math::float2 childDesiredSize(curSlot.getChildWidget()->getFixedSize());
				if (orientation == Orientation::Horizontal)
				{
					myDesiredSize.x += childDesiredSize.x;
					myDesiredSize.y += std::max(childDesiredSize.y, myDesiredSize.y);
				}
				else
				{
					myDesiredSize.x += std::max(childDesiredSize.x, myDesiredSize.x);
					myDesiredSize.y += childDesiredSize.y;	
				}
			}
		}

		float spaceNeededForHandles = std::max(0, numNonCollapsed - 1) * physicalSplitterHandleSize;
		if (orientation == Orientation::Horizontal)
		{
			myDesiredSize.x += spaceNeededForHandles;
		}
		else
		{
			myDesiredSize.y += spaceNeededForHandles;
		}

		return myDesiredSize;
	}

	Splitter::Splitter()
	{
	}
	Splitter::~Splitter()
	{
	}
	void Splitter::init(const BuilderArguments& arguments)
	{
		m_resizeMode = arguments.mresizeMode;
		m_physicalSplitterHandleSize = arguments.mphysicalSplitterHandleSize;
		m_hitDetectionSplitterHandleSize = arguments.mhitDetectionSplitterHandleSize;
		m_orientation = arguments.morientation;
		m_hoveredHandleIndex = -1;
		m_bIsResizing = false;

		m_minSplitterChildLength = arguments.mminimumSlotHeight;
		m_childrens.resize(arguments.mSlots.size());
		for (int32_t slotIndex = 0; slotIndex < arguments.mSlots.size(); ++slotIndex)
		{
			//m_childrens.push_back(arguments.mSlots[slotIndex]);
			std::shared_ptr<Splitter::SplitterSlot> splitterSlot = std::static_pointer_cast<Splitter::SplitterSlot>(arguments.mSlots[slotIndex].m_slot);
			splitterSlot->init(shared_from_this());
			m_childrens.push_back(splitterSlot);
		}
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}

	uint32_t Splitter::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t maxLayerId = 0;
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

		//绘制拖动柄在孩子上面
		maxLayerId += 1;

		//hovered handle index是指向我们孩子数组的索引，但是有一些可能隐藏，并且从我们的数组中丢失，所以，我们必须更新index
		int32_t arrangedHoveredHandleIndex = m_hoveredHandleIndex;
		if (arrangedHoveredHandleIndex != -1 && arrangedWidgetArray.getArrangedWidgetsNumber() != m_childrens.size())
		{
			const std::shared_ptr<Widget>& hoveredChildToFind = m_childrens[m_hoveredHandleIndex]->getChildWidget();
			for (int32_t i = 0; i < arrangedWidgetArray.getArrangedWidgetsNumber(); ++i)
			{
				if (arrangedWidgetArray[i]->getWidget() == hoveredChildToFind)
				{
					arrangedHoveredHandleIndex = i;
				}
			}
		}

		int32_t widgetNumber = arrangedWidgetArray.getArrangedWidgetsNumber() - 1;
		for (int32_t childIndex = 0; childIndex < widgetNumber; ++childIndex)
		{
			const WidgetGeometry& geometryAfterSplitter = arrangedWidgetArray[std::clamp(childIndex + 1, 0, widgetNumber - 1)]->getWidgetGeometry();

			const float halfHitDetectionSplitterHandleSize = std::round(m_hitDetectionSplitterHandleSize / 2.0f);
			const float halfPhysicalSplitterHandleSize = m_physicalSplitterHandleSize;

			math::float2 handleSize;
			math::float2 handlePosition;
			if (m_orientation == Orientation::Horizontal)
			{
				handleSize = math::float2(m_physicalSplitterHandleSize, geometryAfterSplitter.mLocalSize.y);
				handlePosition = math::float2(-m_physicalSplitterHandleSize, 0);
			}
			else
			{
				handleSize = math::float2(geometryAfterSplitter.mLocalSize.x, m_physicalSplitterHandleSize);
				handlePosition = math::float2(0, -m_physicalSplitterHandleSize);
			}

			//悬浮不高亮柄
			if (arrangedHoveredHandleIndex != childIndex)
			{

			}
			else //悬浮高亮柄
			{

			}
		}

		return maxLayerId;
	}

	math::float2 Splitter::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 myDesiredSize = (m_orientation == Orientation::Horizontal)
			? computeDesiredSizeForSplitter<Orientation::Horizontal>(m_physicalSplitterHandleSize, m_childrens)
			: computeDesiredSizeForSplitter<Orientation::Vertical>(m_physicalSplitterHandleSize, m_childrens);
		return myDesiredSize;
	}

	void Splitter::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		std::vector<WidgetGeometry> layoutChildren = arrangedChildrenForLayout(allocatedGeometry);

		for (int32_t childeIndex = 0; childeIndex < m_childrens.size(); ++childeIndex)
		{
			arrangedWidgetArray.pushWidget(layoutChildren[childeIndex], m_childrens[childeIndex]->getChildWidget());
		}
	}

	SlotBase* Splitter::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}

	uint32_t Splitter::getSlotsNumber() const
	{
		return m_childrens.size();
	}

	std::vector<WidgetGeometry> Splitter::arrangedChildrenForLayout(const WidgetGeometry& allottedGeometry) const
	{
		std::vector<WidgetGeometry> result;
		result.resize(m_childrens.size());

		const int32_t axisIndex = (m_orientation == Orientation::Horizontal) ? 0 : 1;

		int32_t numNonCollapsedChildren = 0; //非折叠孩子个数
		int32_t numResizeableChildren = 0;
		float coefficientTotal = 0;//缩放因子总和

		float nonResizebaleSpace = 0;//不可缩放因子，固定大小总和
		{
			for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
			{
				if (m_childrens[childIndex]->getChildWidget()->getVisibility() != Visibility::Collapsed)
				{
					++numNonCollapsedChildren;

					if (m_childrens[childIndex]->m_sizingRule.Get() == Splitter::SizeToContent)
					{
						nonResizebaleSpace += m_childrens[childIndex]->getChildWidget()->getFixedSize()[axisIndex];
					}
					else //SizingRule == Splitter::FractionOfParent
					{
						coefficientTotal += m_childrens[childIndex]->m_sizeValue.Get();
					}
				}
			}
		}

		//所有拖动柄的大小
		const float spaceNeededForHandles = std::max(0, numNonCollapsedChildren - 1) * m_physicalSplitterHandleSize;
		const float resizeableSpace = allottedGeometry.mLocalSize[axisIndex] - spaceNeededForHandles - nonResizebaleSpace;

		//安排孩子水平地或者垂直地
		float xOffset = 0;
		for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
		{
			const std::shared_ptr<SplitterSlot> curSlot = m_childrens[childIndex];

			const float childSpace = (curSlot->m_sizingRule.Get() == Splitter::SizeToContent)
				? curSlot->getChildWidget()->getFixedSize()[axisIndex]
				: resizeableSpace * curSlot->m_sizeValue.Get() / coefficientTotal;

			const Visibility childVisibility = curSlot->getChildWidget()->getVisibility();

			const math::float2 childOffset = m_orientation == Orientation::Horizontal ? math::float2(xOffset, 0) : math::float2(0, xOffset);
			const math::float2 childSize = m_orientation == Orientation::Vertical ? math::float2(childSpace, allottedGeometry.mLocalSize.x) : math::float2(allottedGeometry.mLocalSize.x, childSpace);

			//child size and child offset
			result.push_back(WidgetGeometry::makeRoot(math::float2(childSize), math::affine2(math::float2x2::diagonal(1.0f), childOffset)));

			if (childVisibility != Visibility::Collapsed)
			{
				xOffset += std::round(childSpace + m_physicalSplitterHandleSize);
			}
		}

		return result;
	}

}