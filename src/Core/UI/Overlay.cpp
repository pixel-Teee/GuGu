#include <pch.h>

#include "Overlay.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"


namespace GuGu {
	Overlay::Overlay()
	{
	}
	Overlay::~Overlay()
	{
	}
	void Overlay::init(const BuilderArguments& arguments)
	{
		for (size_t i = 0; i < arguments.mSlots.size(); ++i)
		{
			m_childrens.push_back(std::static_pointer_cast<OverlaySlot>(arguments.mSlots[i].m_slot));
			m_childrens[i]->init(shared_from_this(), arguments.mSlots[i]);
		}
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;
	}
	uint32_t Overlay::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		uint32_t maxLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);

			if (childIndex > 0)
			{
				++maxLayerId;
			}

			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), maxLayerId);
				const int32_t overlaySlotPadding = 10;
				maxLayerId = curWidgetsMaxLayerId + std::min(std::max(((int32_t)curWidgetsMaxLayerId - (int32_t)maxLayerId) / overlaySlotPadding, 1) * overlaySlotPadding, 100);
			}
		}

		return maxLayerId;
	}
	GuGu::math::float2 Overlay::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		math::float2 maxSize(0.0f, 0.0f);
		for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
		{
			const OverlaySlot& curSlot = *m_childrens[childIndex];
			const Visibility childVisiblity = curSlot.getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{			
				math::float2 childSize = curSlot.getChildWidget()->getFixedSize();
				math::float2 childDesiredSize = math::float2(childSize.x, childSize.y) + curSlot.getPadding().getFixedSize();
				maxSize.x = std::max(maxSize.x, childDesiredSize.x);
				maxSize.y = std::max(maxSize.y, childDesiredSize.y);
			}			
		}

		return math::float2(maxSize.x, maxSize.y);
	}
	void Overlay::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
		for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
		{
			const OverlaySlot& curChild = *m_childrens[childIndex];
			//todo:add visibility check
			const Visibility childVisibility = getSlot(childIndex)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				const Padding slotPadding(curChild.getPadding());

				AlignmentArrangeResult xResult = AlignChild<Orientation::Horizontal>(curChild, allocatedGeometry.getLocalSize().x, slotPadding);
				AlignmentArrangeResult yResult = AlignChild<Orientation::Vertical>(curChild, allocatedGeometry.getLocalSize().y, slotPadding);

				arrangedWidgetArray.pushWidget(allocatedGeometry.getChildGeometry(math::float2(xResult.m_size, yResult.m_size), math::float2(xResult.m_offset, yResult.m_offset)), curChild.getChildWidget());
			}
		}
	}
	SlotBase* Overlay::getSlot(uint32_t index) const
	{
		return m_childrens[index].get();
	}
	uint32_t Overlay::getSlotsNumber() const
	{
		return m_childrens.size();
	}
	Overlay::OverlaySlot::SlotBuilderArguments Overlay::Slot()
	{
		return OverlaySlot::SlotBuilderArguments(std::make_shared<OverlaySlot>());
	}
	Overlay::ScopedWidgetSlotArguments<Overlay::OverlaySlot> Overlay::addSlot(int32_t zOrder)
	{
		int32_t index = -1;
		std::shared_ptr<OverlaySlot> newSlot = std::make_shared<OverlaySlot>();
		if (zOrder == -1)
		{
			zOrder = (m_childrens.size() == 0) ? 0 : (m_childrens[m_childrens.size() - 1]->m_zOrder + 1);

			//this->m_childrens.push_back(newSlot);
		}
		else
		{
			bool bFoundSlot = false;
			int32_t curSlotIndex = 0;
			for (; curSlotIndex < m_childrens.size(); ++curSlotIndex)
			{
				const OverlaySlot& curSlot = *m_childrens[curSlotIndex];
				if (zOrder < curSlot.m_zOrder)
				{
					bFoundSlot = true;
					break;
				}
			}
			index = curSlotIndex;
			//this->m_childrens.insert(m_childrens.begin() + curSlotIndex, newSlot);
		}

		//newSlot->m_zOrder = zOrder;

		//OverlaySlot::SlotBuilderArguments slotArguments(newSlot);
		//newSlot->init(shared_from_this(), slotArguments);
		
		return Overlay::ScopedWidgetSlotArguments<Overlay::OverlaySlot>(std::make_shared<OverlaySlot>(zOrder), this->m_childrens, index, std::static_pointer_cast<Overlay>(shared_from_this()));
	}
	void Overlay::removeSlot(int32_t zOrder)
	{
		if (zOrder != -1)
		{
			for (int32_t childIndex = 0; childIndex < m_childrens.size(); ++childIndex)
			{
				if (m_childrens[childIndex]->m_zOrder == zOrder)
				{
					m_childrens.erase(m_childrens.begin() + childIndex);
					return;
				}
			}
		}
		else if (m_childrens.size() > 0)
		{
			m_childrens.pop_back();
		}
	}
	bool Overlay::removeSlot(std::shared_ptr<Widget> widget)
	{
		for (int32_t curSlotIndex = 0; curSlotIndex < m_childrens.size(); ++curSlotIndex)
		{
			const OverlaySlot& curSlot = *m_childrens[curSlotIndex];
			if (curSlot.getChildWidget() == widget)
			{
				m_childrens.erase(m_childrens.begin() + curSlotIndex);
				break;
				return true;
			}
		}
		return false;
	}
}