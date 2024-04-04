#include <pch.h>

#include "NodePanel.h"
#include "LayoutUtils.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	Node::Node()
	{
	}
	Node::~Node()
	{
	}
	void Node::init(const BuilderArguments& arguments)
	{
		for (size_t i = 0; i < arguments.mSlots.size(); ++i)
		{
			m_children.push_back(std::static_pointer_cast<NodeSlot>(arguments.mSlots[i].m_slot));
			m_children[i]->init(arguments.mSlots[i]);
		}
	}
	uint32_t Node::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedChildren(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedChildren);

		int32_t maxLayerId = layer;
		for (int32_t childIndex = 0; childIndex < arrangedChildren.getArrangedWidgetsNumber(); ++childIndex)
		{
			const ArrangedWidget& curWidget = *arrangedChildren.getArrangedWidget(childIndex);

			const int32_t curWidgetMaxLayerId = curWidget.getWidget()->generateElement(paintArgs, cullingRect, elementList, allocatedGeometry, layer);

			maxLayerId = std::max(maxLayerId, curWidgetMaxLayerId);
		}

		return maxLayerId;
	}
	GuGu::math::float2 Node::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		for (int32_t childIndex = 0; childIndex < m_children.size(); ++childIndex)
		{
			if (m_children[childIndex])
			{
				//todo:fix this
				const NodeSlot& centerZone = *m_children[childIndex];

				return (centerZone.getChildWidget()->getFixedSize() + centerZone.getPadding().getFixedSize());
			}
		}

		return math::float2(0.0f, 0.0f);
	}
	void Node::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		for (int32_t childIndex = 0; childIndex < m_children.size(); ++childIndex)
		{
			const NodeSlot& curChild = *m_children[childIndex];

			const Padding slotPadding(curChild.getPadding());

			math::float2 Size;

			if (curChild.m_size.IsSet())
			{
				Size = curChild.m_size.Get();
			}
			else
			{
				AlignmentArrangeResult xResult = AlignChild<Orientation::Horizontal>(curChild, allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yResult = AlignChild<Orientation::Vertical>(curChild, allocatedGeometry.getLocalSize().y);
				Size = math::float2(xResult.m_size, yResult.m_size);
			}
			const WidgetGeometry childGeom = allocatedGeometry.getChildGeometry(Size, curChild.m_offset.Get(), allocatedGeometry.getAccumulateTransform());
			arrangedWidgetArray.pushWidget(childGeom, curChild.getChildWidget());
		}
	}
	SlotBase* Node::getSlot(uint32_t index)
	{
		return m_children[index].get();
	}
	uint32_t Node::getSlotsNumber()
	{
		return m_children.size();
	}
	NodePanel::NodePanel()
	{
	}
	NodePanel::~NodePanel()
	{
	}
	math::float2 NodePanel::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2(160.0f, 120.f);
	}
	void NodePanel::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray)
	{
		for (int32_t childIndex = 0; childIndex < m_children.size(); ++childIndex)
		{
			const std::shared_ptr<Node>& someChild = std::static_pointer_cast<Node>(m_children[childIndex]->getChildWidget());

			arrangedWidgetArray.pushWidget(allocatedGeometry.getChildGeometry(someChild->getFixedSize(), someChild->getPosition(), allocatedGeometry.getAccumulateTransform()), someChild);
		}
	}
	SlotBase* NodePanel::getSlot(uint32_t index)
	{
		return m_children[index].get();
	}
	uint32_t NodePanel::getSlotsNumber()
	{
		return m_children.size();
	}
}