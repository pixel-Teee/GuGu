#include <pch.h>

#include "ConstraintCanvas.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	//canvas

	ConstraintCanvas::ConstraintCanvas()
	{

	}

	ConstraintCanvas::~ConstraintCanvas()
	{

	}

	void ConstraintCanvas::init(const BuilderArguments& arguments)
	{
		//add slots
		m_childrens.resize(arguments.mSlots.size());
		for (int32_t slotIndex = 0; slotIndex < arguments.mSlots.size(); ++slotIndex)
		{
			std::shared_ptr<ConstraintCanvas::CanvasSlot> uniformSlot = std::static_pointer_cast<ConstraintCanvas::CanvasSlot>(arguments.mSlots[slotIndex].m_slot);
			CanvasSlot::SlotBuilderArguments builderArguments = arguments.mSlots[slotIndex];
			uniformSlot->init(shared_from_this(), builderArguments);
			m_childrens[slotIndex] = uniformSlot;
		}
		std::stable_sort(m_childrens.begin(), m_childrens.end(), [](const std::shared_ptr<CanvasSlot>& lhs, const std::shared_ptr<CanvasSlot>& rhs) {
			int32_t aZOrder = lhs->m_zOrder.Get();
			int32_t bZOrder = rhs->m_zOrder.Get();
			return aZOrder == bZOrder ? (lhs.get() < rhs.get()) : aZOrder < bZOrder;
		});
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}

	ConstraintCanvas::CanvasSlot::SlotBuilderArguments ConstraintCanvas::Slot()
	{
		return CanvasSlot::SlotBuilderArguments(std::make_shared<CanvasSlot>());
	}

	uint32_t ConstraintCanvas::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		uint32_t maxLayerId = layer;
		uint32_t childLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		ArrangedChildLayers childLayers;
		arrangeLayeredChildren(allocatedGeometry, arrangedWidgetArray, childLayers);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				//bool in child layers tell us whether to paint the next child in front of all previous
				if (childLayers[childIndex])
				{
					childLayerId = maxLayerId + 1;
				}

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), childLayerId);
				maxLayerId = std::max(maxLayerId, curWidgetsMaxLayerId);
			}
		}

		return maxLayerId;
	}

	math::float2 ConstraintCanvas::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 finalFixedSize(0, 0);
		for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
		{
			const std::shared_ptr<CanvasSlot>& curChild = m_childrens[childIndex];
			const std::shared_ptr<Widget>& widget = curChild->getChildWidget();
			const Visibility childVisibility = widget->getVisibility();

			if (childVisibility != Visibility::Collapsed)
			{
				const Padding offset = curChild->m_offsetAttr.Get();
				const math::float2 alignment = curChild->m_alignmentAttr.Get();
				const Anchors anchors = curChild->m_anchorsAttr.Get();

				const math::float2 slotSize = math::float2(offset.right, offset.bottom);

				const bool autoSize = curChild->m_bAutoSizeAttr.Get();

				const math::float2 fixedSize = autoSize ? widget->getFixedSize() : slotSize;

				const bool bIsDockedHorizontally = (anchors.m_minimum.x == anchors.m_maximum.x) && (anchors.m_minimum.x == 0 || anchors.m_minimum.x == 1);
				const bool bIsDockedVertically = (anchors.m_minimum.y == anchors.m_maximum.y) && (anchors.m_minimum.y == 0 || anchors.m_minimum.y == 1);

				finalFixedSize.x = std::max(finalFixedSize.x, fixedSize.x + (bIsDockedHorizontally ? std::abs(offset.left) : 0.0f));
				finalFixedSize.y = std::max(finalFixedSize.y, fixedSize.y + (bIsDockedVertically ? std::abs(offset.top) : 0.0f));
			}
		}

		return finalFixedSize;
	}

	void ConstraintCanvas::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		ArrangedChildLayers childLayers;
		arrangeLayeredChildren(allocatedGeometry, arrangedWidgetArray, childLayers);
	}

	SlotBase* ConstraintCanvas::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}

	uint32_t ConstraintCanvas::getSlotsNumber() const
	{
		return m_childrens.size();
	}

	ConstraintCanvas::ScopedWidgetSlotArguments<ConstraintCanvas::CanvasSlot> ConstraintCanvas::addSlot()
	{
		//todo:fix this
		return
			ConstraintCanvas::ScopedWidgetSlotArguments<ConstraintCanvas::CanvasSlot>
			(std::make_shared<CanvasSlot>(),
			m_childrens,
			-1,
			std::static_pointer_cast<ConstraintCanvas>(shared_from_this()));
	}

	void ConstraintCanvas::arrangeLayeredChildren(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray, ArrangedChildLayers& arrangedChildLayers) const
	{
		if (m_childrens.size())
		{
			const bool bExplicitChildZOrder = 0;

			float lastZOrder = -FLT_MIN;

			for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
			{
				const std::shared_ptr<CanvasSlot>& curChild = m_childrens[childIndex];
				const std::shared_ptr<Widget>& widget = curChild->getChildWidget();
				const Visibility childVisibility = widget->getVisibility();

				if (arrangedWidgetArray.accepts(childVisibility))
				{
					const Padding offset = curChild->m_offsetAttr.Get();
					const math::float2 alignment = curChild->m_alignmentAttr.Get();
					const Anchors anchors = curChild->m_anchorsAttr.Get();
					const bool autoSize = curChild->m_bAutoSizeAttr.Get();

					const Padding anchorPixels = Padding(
						anchors.m_minimum.x * allocatedGeometry.getLocalSize().x,
						anchors.m_minimum.y * allocatedGeometry.getLocalSize().y,
						anchors.m_maximum.x * allocatedGeometry.getLocalSize().x,
						anchors.m_maximum.y * allocatedGeometry.getLocalSize().y
					);

					const bool bIsHorizontalStretch = anchors.m_minimum.x != anchors.m_maximum.x;
					const bool bIsVerticalStretch = anchors.m_minimum.y != anchors.m_maximum.y;

					const math::float2 slotSize = math::float2(offset.right, offset.bottom);

					const math::float2 Size = autoSize ? widget->getFixedSize() : slotSize;

					math::float2 alignmentOffset = Size * alignment;//the offset based on the pivot position

					math::float2 localPosition, localSize;

					if (bIsHorizontalStretch)
					{
						localPosition.x = anchorPixels.left + offset.left;
						localSize.x = anchorPixels.right - localPosition.x - offset.right;
					}
					else
					{
						localPosition.x = anchorPixels.left + offset.left - alignmentOffset.x;
						localSize.x = Size.x;
					}

					if (bIsVerticalStretch)
					{
						localPosition.y = anchorPixels.top + offset.top;
						localSize.y = anchorPixels.bottom - localPosition.y - offset.bottom;
					}
					else
					{
						localPosition.y = anchorPixels.top + offset.top - alignmentOffset.y;
						localSize.y = Size.y;
					}

					//add widget
					WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(
						localSize,
						localPosition);

					arrangedWidgetArray.pushWidget(childGeometry, widget);

					bool bNewLayer = true;
					if (bExplicitChildZOrder)
					{
						bNewLayer = false;
						if (curChild->m_zOrder.Get() > lastZOrder + math::epsilon)
						{
							if (arrangedChildLayers.size() > 0)
							{
								bNewLayer = true;
							}
							lastZOrder = curChild->m_zOrder.Get();
						}
					}
					arrangedChildLayers.push_back(bNewLayer);
				}
			}
		}
	}

}