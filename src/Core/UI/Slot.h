#pragma once

#include "BasicElement.h"

namespace GuGu {
	class Widget;
	class SlotBase
	{
	public:
		SlotBase() {}

		SlotBase(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment);

		virtual ~SlotBase();

		virtual std::shared_ptr<Widget> getChildWidget() const;

		void setChildWidget(std::shared_ptr<Widget> widget);

		Padding getPadding() const;

		HorizontalAlignment getHorizontalAlignment() const;

		VerticalAlignment getVerticalAlignment() const;

		void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);

		void setVerticalAlignment(VerticalAlignment verticalAlignment);

		void setPadding(Padding newPadding);

	protected:
		HorizontalAlignment m_horizontalAlignment;
		VerticalAlignment m_verticalAlignment;
		Padding m_padding;
		std::shared_ptr<Widget> m_childWidget;
	};
	template<typename SlotType>
	class Slot : public SlotBase
	{
	public:
		Slot() {}

		Slot(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment) : SlotBase(inHorizontalAlignment, inVerticalAlignment) {}

		virtual ~Slot() = default;

		struct SlotBuilderArguments
		{
			SlotBuilderArguments(std::shared_ptr<Slot<SlotType>> inSlot)
				: m_slot(inSlot) {}
			virtual ~SlotBuilderArguments() {};

			typename SlotType::SlotBuilderArguments& operator()(std::shared_ptr<Widget> childWidget)
			{
				m_slot->setChildWidget(childWidget);
				//return static_cast<typename SlotType::SlotArguments&>(*this);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& Padding(Padding inPadding)
			{
				m_slot->setPadding(inPadding);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& HorizontalAlignment(HorizontalAlignment horizontalAlignment)
			{
				m_slot->setHorizontalAlignment(horizontalAlignment);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& VerticalAlignment(VerticalAlignment verticalAlignment)
			{
				m_slot->setVerticalAlignment(verticalAlignment);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& Me()
			{
				return static_cast<typename SlotType::SlotBuilderArguments&>(*this);
			}
			std::shared_ptr<Slot<SlotType>> m_slot;
		};		
	};
	
	class SingleChildSlot : public Slot<SingleChildSlot>
	{
	public:
		SingleChildSlot() : Slot(HorizontalAlignment::Center, VerticalAlignment::Center) {}

		SingleChildSlot(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment)
			: Slot(inHorizontalAlignment, inVerticalAlignment)
		{}
		virtual ~SingleChildSlot() {}
	};
}