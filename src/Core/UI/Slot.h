#pragma once

#include "BasicElement.h"

namespace GuGu {
	class Widget;
	class Slot
	{
	public:
		Slot();

		Slot(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment);

		virtual ~Slot();

		struct SlotBuilderArguments
		{
			SlotBuilderArguments(std::shared_ptr<Slot> inSlot)
				: m_slot(inSlot) {}
			~SlotBuilderArguments() = default;

			SlotBuilderArguments& operator()(std::shared_ptr<Widget> childWidget)
			{
				m_slot->setChildWidget(childWidget);
				return *this;
			}

			SlotBuilderArguments& Padding(Padding inPadding)
			{
				m_slot->setPadding(inPadding);
				return *this;
			}

			SlotBuilderArguments& HorizontalAlignment(HorizontalAlignment horizontalAlignment)
			{
				m_slot->setHorizontalAlignment(horizontalAlignment);
				return *this;
			}

			SlotBuilderArguments& VerticalAlignment(VerticalAlignment verticalAlignment)
			{
				m_slot->setVerticalAlignment(verticalAlignment);
				return *this;
			}
			std::shared_ptr<Slot> m_slot;
		};
		
		virtual std::shared_ptr<Widget> getChildWidget();

		void setChildWidget(std::shared_ptr<Widget> widget);

		Padding getPadding();

		HorizontalAlignment getHorizontalAlignment();

		VerticalAlignment getVerticalAlignment();

		void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);

		void setVerticalAlignment(VerticalAlignment verticalAlignment);

		void setPadding(Padding newPadding);

	protected:
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		Padding m_padding;
		std::shared_ptr<Widget> m_childWidget;
	};
}