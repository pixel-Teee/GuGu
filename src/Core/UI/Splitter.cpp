#include <pch.h>

#include "Splitter.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "ElementList.h"
#include "Brush.h"

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

	template<Orientation orientation>
	int32_t Splitter::getHandleBeingResizedFromMousePosition(float inPhysicalSplitterHandleSize, float hitDetectionSplitterHandleSize, math::float2 localMousePos, const std::vector<WidgetGeometry>& childGeometries)
	{
		const int32_t axisIndex = (orientation == Orientation::Horizontal) ? 0 : 1;
		const float halfHitDetectionSplitterHandleSize = (hitDetectionSplitterHandleSize / 2);
		const float halfPhysicalSplitterHandleSize = (inPhysicalSplitterHandleSize / 2);

		for (int32_t childIndex = 1; childIndex < childGeometries.size(); ++childIndex)
		{
			math::box2 prevChildRect = childGeometries[childIndex - 1].getAbsoluteRect();
			math::float2 nextChildOffset = childGeometries[childIndex].getAbsolutePosition();
			//topleft + size
			float prevBound = prevChildRect.getCorner(0)[axisIndex] + prevChildRect.diagonal()[axisIndex] - halfHitDetectionSplitterHandleSize + halfPhysicalSplitterHandleSize;
			float nextBound = nextChildOffset[axisIndex] + halfHitDetectionSplitterHandleSize - halfPhysicalSplitterHandleSize;

			if(localMousePos[axisIndex] > prevBound && localMousePos[axisIndex] < nextBound)
			{
				return childIndex - 1;
			}
		}

		return -1;
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
		m_style = arguments.msplitterStyle;

		m_minSplitterChildLength = arguments.mminimumSlotHeight;
		m_childrens.resize(arguments.mSlots.size());
		for (int32_t slotIndex = 0; slotIndex < arguments.mSlots.size(); ++slotIndex)
		{
			//m_childrens.push_back(arguments.mSlots[slotIndex]);
			std::shared_ptr<Splitter::SplitterSlot> splitterSlot = std::static_pointer_cast<Splitter::SplitterSlot>(arguments.mSlots[slotIndex].m_slot);
			SplitterSlot::SlotBuilderArguments builderArguments = arguments.mSlots[slotIndex];
			splitterSlot->init(shared_from_this(), builderArguments);
			m_childrens[slotIndex] = splitterSlot;
		}
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}

	Splitter::SplitterSlot::SlotBuilderArguments Splitter::Slot()
	{
		return Splitter::SplitterSlot::SlotBuilderArguments(std::make_shared<SplitterSlot>());
	}

	Splitter::ScopedWidgetSlotArguments<Splitter::SplitterSlot> Splitter::addSlot(int32_t atIndex /*= -1*/)
	{
		return Splitter::ScopedWidgetSlotArguments<Splitter::SplitterSlot>(std::make_shared<SplitterSlot>(), m_childrens, atIndex, std::static_pointer_cast<Splitter>(shared_from_this()));
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

		int32_t widgetNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (int32_t childIndex = 0; childIndex < widgetNumber - 1; ++childIndex)
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
				ElementList::addBoxElement(elementList, 
					geometryAfterSplitter.getChildGeometry(handleSize, handlePosition),
					m_style->m_handleNormalBrush->getTintColor(),
					m_style->m_handleNormalBrush,
					maxLayerId);
			}
			else //悬浮高亮柄
			{
				ElementList::addBoxElement(elementList,
					geometryAfterSplitter.getChildGeometry(handleSize, handlePosition),
					m_style->m_handleHighlightBrush->getTintColor(),
					m_style->m_handleHighlightBrush,
					maxLayerId);
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
			arrangedWidgetArray.pushWidget(allocatedGeometry.getChildGeometry(layoutChildren[childeIndex].getLocalSize(), layoutChildren[childeIndex].getLocalPosition()), m_childrens[childeIndex]->getChildWidget());
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
			const math::float2 childSize = m_orientation == Orientation::Horizontal ? math::float2(childSpace, allottedGeometry.mLocalSize.y) : math::float2(allottedGeometry.mLocalSize.x, childSpace);

			//child size and child offset
			result[childIndex] = WidgetGeometry::makeRoot(math::float2(childSize), math::affine2(math::float2x2::diagonal(1.0f), childOffset));

			if (childVisibility != Visibility::Collapsed)
			{
				xOffset += std::round(childSpace + m_physicalSplitterHandleSize);
			}
		}

		return result;
	}

	Reply Splitter::OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton && m_hoveredHandleIndex != -1)
		{
			m_bIsResizing = true;
			return Reply::Handled().captureMouse(shared_from_this());
		}
		else
		{
			return Reply::Unhandled();
		}
	}

	Reply Splitter::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton && m_bIsResizing == true)
		{
			m_bIsResizing = false;
			return Reply::Handled().releaseMouseCapture();
		}
		else
		{
			return Reply::Unhandled();
		}
	}

	Reply Splitter::OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
	{
		const math::float2 localMousePosition = myGeometry.absoluteToLocal(inMouseEvent.m_screenSpacePosition);

		std::vector<WidgetGeometry> layoutChildren = arrangedChildrenForLayout(myGeometry);

		if (m_bIsResizing)
		{
			math::float2 cursorDelta = inMouseEvent.getCursorDelta();
			if(cursorDelta.x != 0 || cursorDelta.y != 0)
			{
				handleResizingByMousePosition(m_orientation, m_physicalSplitterHandleSize, m_resizeMode, m_hoveredHandleIndex, localMousePosition, m_childrens, layoutChildren);
			}

			return Reply::Handled();
		}
		else
		{
			//检测哪一个柄我们当前悬浮
			m_hoveredHandleIndex = (m_orientation == Orientation::Horizontal)
				? getHandleBeingResizedFromMousePosition<Orientation::Horizontal>(m_physicalSplitterHandleSize, m_hitDetectionSplitterHandleSize, localMousePosition, layoutChildren)
				: getHandleBeingResizedFromMousePosition<Orientation::Vertical>(m_physicalSplitterHandleSize, m_hitDetectionSplitterHandleSize, localMousePosition, layoutChildren);
				
			if (m_hoveredHandleIndex != -1)
			{
				if (findResizeableSlotBeforeHandle(m_hoveredHandleIndex, m_childrens) <= -1 || findResizeableSlotAfterHandle(m_hoveredHandleIndex, m_childrens) >= m_childrens.size())
				{
					m_hoveredHandleIndex = -1;
				}
			}

			return Reply::Unhandled();
		}
	}

	void Splitter::handleResizingByMousePosition(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, const math::float2& localMousePos, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries)
	{
		const int32_t axisIndex = (orientation == Orientation::Horizontal) ? 0 : 1;

		//算出鼠标位置距离柄的偏移
		const float handlePos = childGeometries[draggedHandle + 1].getAbsolutePosition()[axisIndex] - physicalSplitterHandleSize;
		float delta = localMousePos[axisIndex] - handlePos;

		handleResizingDelta(orientation, physicalSplitterHandleSize, resizeMode, draggedHandle, delta, children, childGeometries);
	}

	void Splitter::handleResizingDelta(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, float delta, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries)
	{
		const int32_t numChildren = children.size();
		const int32_t axisIndex = (orientation == Orientation::Horizontal) ? 0 : 1;

		std::vector<int32_t> slotsAfterDragHandleIndicies;
		if (resizeMode == SplitterResizeMode::FixedPosition) 
		{
			const int32_t slotAfterDragHandle = findResizeableSlotAfterHandle(draggedHandle, children);

			if (slotAfterDragHandle < numChildren)
			{
				slotsAfterDragHandleIndicies.push_back(slotAfterDragHandle);
			}
		}
		else if (resizeMode == SplitterResizeMode::Fill || resizeMode == SplitterResizeMode::FixedSize)
		{
			findAllResizeableSlotsAfterHandle(draggedHandle, children, /*out*/slotsAfterDragHandleIndicies);
		}

		const int32_t numSlotsAfterDragHandle = slotsAfterDragHandleIndicies.size();
		if (numSlotsAfterDragHandle)
		{
			struct SlotInfo
			{
				Splitter::SplitterSlot* slot;
				const WidgetGeometry* geometry;
				float newSize;
			};

			std::vector<SlotInfo> slotsAfterDragHandle;
			for (int32_t slotIndex = 0; slotIndex < numSlotsAfterDragHandle; ++slotIndex)
			{
				SlotInfo slotInfo;

				slotInfo.slot = children[slotsAfterDragHandleIndicies[slotIndex]].get();
				slotInfo.geometry = &childGeometries[slotsAfterDragHandleIndicies[slotIndex]];
				slotInfo.newSize = clampChild(slotInfo.geometry->getAbsoluteSize()[axisIndex]);

				slotsAfterDragHandle.push_back(slotInfo);
			}

			const int32_t slotBeforeDragHandle = findResizeableSlotBeforeHandle(draggedHandle, children);
			SplitterSlot& prevChild = *children[slotBeforeDragHandle];
			const WidgetGeometry& prevChildGeom = childGeometries[slotBeforeDragHandle];

			const float prevChildLength = prevChildGeom.getAbsoluteSize()[axisIndex];
			float newPrevChildLength = clampChild(prevChildLength + delta);
			delta = newPrevChildLength - prevChildLength;

			float unUsedDelta = delta;

			for (float distributionCount = 0; distributionCount < numSlotsAfterDragHandle && unUsedDelta != 0; ++distributionCount)
			{
				float dividedDelta = resizeMode != SplitterResizeMode::FixedSize ? unUsedDelta / numSlotsAfterDragHandle : unUsedDelta;

				unUsedDelta = 0;

				int32_t slotIndex = 0;

				if (resizeMode == SplitterResizeMode::FixedSize)
				{
					slotIndex = numSlotsAfterDragHandle - 1;
				}

				for (; slotIndex < numSlotsAfterDragHandle; ++slotIndex)
				{
					SlotInfo& slotInfo = slotsAfterDragHandle[slotIndex];
					float currentSize = slotInfo.newSize;
					slotInfo.newSize = clampChild(currentSize - dividedDelta);

					unUsedDelta += slotInfo.newSize - (currentSize - dividedDelta);
				}
			}

			delta = delta - unUsedDelta;

			newPrevChildLength = clampChild(prevChildLength + delta);

			{
				float totalLength = newPrevChildLength;
				float totalStretchCoefficients = prevChild.m_sizeValue.Get();

				for (int32_t slotIndex = 0; slotIndex < numSlotsAfterDragHandle; ++slotIndex)
				{
					SlotInfo slotInfo = slotsAfterDragHandle[slotIndex];

					totalLength += slotInfo.newSize;
					totalStretchCoefficients += slotInfo.slot->m_sizeValue.Get();
				}

				const float newPrevChildSize = (totalStretchCoefficients * newPrevChildLength / totalLength);

				if (prevChild.m_onSlotResizedHandler)
				{
					prevChild.m_onSlotResizedHandler(newPrevChildSize);
				}
				else
				{
					prevChild.m_sizeValue = newPrevChildSize;
				}

				for (int32_t slotIndex = 0; slotIndex < numSlotsAfterDragHandle; ++slotIndex)
				{
					SlotInfo slotInfo = slotsAfterDragHandle[slotIndex];

					const float newNextChildSize = (totalStretchCoefficients * slotInfo.newSize / totalLength);

					if (slotInfo.slot->m_onSlotResizedHandler)
					{
						slotInfo.slot->m_onSlotResizedHandler(newNextChildSize);
					}
					else
					{
						slotInfo.slot->m_sizeValue = newNextChildSize;
					}
				}
			}
		}
	}

	void Splitter::handleResizingBySize(Orientation orientation, const float physicalSplitterHandleSize, const SplitterResizeMode::Type resizeMode, int32_t draggedHandle, const math::float2& desiredSize, std::vector<std::shared_ptr<Splitter::SplitterSlot>>& children, const std::vector<WidgetGeometry>& childGeometries)
	{
	}

	int32_t Splitter::findResizeableSlotBeforeHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children)
	{
		int32_t slotBeforeDragHandle = draggedHandle;
		while (slotBeforeDragHandle >= 0 && (children[slotBeforeDragHandle]->getChildWidget()->getVisibility() == Visibility::Collapsed || children[slotBeforeDragHandle]->m_sizingRule.Get() == Splitter::SizeToContent))
		{
			--slotBeforeDragHandle;
		}

		return slotBeforeDragHandle;
	}

	void Splitter::findAllResizeableSlotsAfterHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children, std::vector<int32_t>& outSlotIndices)
	{
		const int32_t numChildren = children.size();

		for (int32_t slotIndex = draggedHandle + 1; slotIndex < numChildren; ++slotIndex)
		{
			if (children[slotIndex]->getChildWidget()->getVisibility() == Visibility::Collapsed || children[slotIndex]->m_sizingRule.Get() == Splitter::SizeToContent)
			{
				continue;
			}

			outSlotIndices.push_back(slotIndex);
		}
	}

	int32_t Splitter::findResizeableSlotAfterHandle(int32_t draggedHandle, const std::vector<std::shared_ptr<SplitterSlot>>& children)
	{
		const int32_t numChildren = children.size();

		int32_t slotAfterDragHandle = draggedHandle + 1;
		{
			//找到可以移动的槽
			while (slotAfterDragHandle < numChildren && (children[slotAfterDragHandle]->getChildWidget()->getVisibility() == Visibility::Collapsed || children[slotAfterDragHandle]->m_sizingRule.Get() == Splitter::SizeToContent))
			{
				++slotAfterDragHandle;
			}
		}

		return slotAfterDragHandle;
	}

	float Splitter::clampChild(float proposedSize)
	{
		return std::max(m_minSplitterChildLength, proposedSize);
	}

}