#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"
#include "Attribute.h"

namespace GuGu {
	class NodePanel : public Widget
	{
	public:
		class Node : public Widget
		{
		public:
			class NodeSlot : public Slot<NodeSlot>
			{
			public:
				friend Node;

			private:
				Attribute<Padding> m_slotPadding;
				Attribute<math::float2> m_offset;
				Attribute<math::float2> m_size;
				Attribute<bool> m_bAllowScale;
			};
		
		private:
			std::vector<NodeSlot> m_children;
			std::weak_ptr<NodePanel> m_parentPanelPtr;
		};
	};
}