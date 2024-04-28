#pragma once

#include "BasicElement.h"

namespace GuGu {
	class Widget;
	class SlotBase
	{
	public:
		SlotBase() {}

		SlotBase(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment, Padding inPadding);

		virtual ~SlotBase();

		virtual std::shared_ptr<Widget> getChildWidget() const;

		void setChildWidget(std::shared_ptr<Widget> widget);

		Padding getPadding() const;

		HorizontalAlignment getHorizontalAlignment() const;

		VerticalAlignment getVerticalAlignment() const;

		void setHorizontalAlignment(HorizontalAlignment horizontalAlignment);

		void setVerticalAlignment(VerticalAlignment verticalAlignment);

		void setPadding(Padding newPadding);

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

		Slot(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment, Padding inPadding) : SlotBase(inHorizontalAlignment, inVerticalAlignment, inPadding) {}

		virtual ~Slot() = default;

		struct SlotBuilderArguments
		{
			SlotBuilderArguments(std::shared_ptr<SlotType> inSlot)
				: m_slot(inSlot){}
			virtual ~SlotBuilderArguments() {};

			typename SlotType::SlotBuilderArguments& operator()(std::shared_ptr<Widget> childWidget)
			{
				m_slot->setChildWidget(childWidget);
				//childWidget->setParentWidget(m_parentWidget);
				//return static_cast<typename SlotType::SlotArguments&>(*this);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& setPadding(Padding inPadding)
			{
				m_slot->setPadding(inPadding);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& setHorizontalAlignment(HorizontalAlignment horizontalAlignment)
			{
				m_slot->setHorizontalAlignment(horizontalAlignment);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& setVerticalAlignment(VerticalAlignment verticalAlignment)
			{
				m_slot->setVerticalAlignment(verticalAlignment);
				return Me();
			}

			typename SlotType::SlotBuilderArguments& Me()
			{
				return static_cast<typename SlotType::SlotBuilderArguments&>(*this);
			}

			typename SlotType::SlotBuilderArguments& expose(SlotType*& outVarToInit)//slot type:horizontal box slot *&
			{
				outVarToInit = m_slot.get();
				return Me();
			}

			std::shared_ptr<SlotType> m_slot;
			//std::shared_ptr<Slot<SlotType>> m_slot;
		};		

		std::weak_ptr<Widget> m_parentWidget;
	};
	
	class SingleChildSlot : public Slot<SingleChildSlot>
	{
	public:
		SingleChildSlot() : Slot(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f)) {}

		SingleChildSlot(HorizontalAlignment inHorizontalAlignment, VerticalAlignment inVerticalAlignment, Padding inPadding)
			: Slot(inHorizontalAlignment, inVerticalAlignment, inPadding)
		{}
		virtual ~SingleChildSlot() {}
	};
}