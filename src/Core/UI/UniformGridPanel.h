#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "Slot.h"

namespace GuGu {
	class UniformGridPanel : public Widget
	{
	public:
		UniformGridPanel();

		virtual ~UniformGridPanel();

		//------pre-child info------
		class UniformGridSlot : public Slot<UniformGridSlot>
		{
		public:
			UniformGridSlot()
			{
				m_column = m_row = 0;
			}
			UniformGridSlot(int32_t inColumn, int32_t inRow)
			: m_column(inColumn)
			, m_row(inRow)
			{
				//
			}

			virtual ~UniformGridSlot() = default;

			void init(std::shared_ptr<Widget> inParentWidget, const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);
			}

			struct SlotBuilderArguments : public Slot<UniformGridSlot>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<UniformGridSlot> inSlot)
					: Slot<UniformGridSlot>::SlotBuilderArguments(inSlot)
				{}
				
				SlotBuilderArguments& Column(int32_t inColumn)
				{
					column = inColumn;
					return *this;
				}

				SlotBuilderArguments& Row(int32_t inRow)
				{
					row = inRow;
					return *this;
				}

				int32_t column;
				int32_t row;
			};

			int32_t m_column;
			int32_t m_row;
		};
		//------pre-child info------

		struct BuilderArguments : public Arguments<UniformGridPanel>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(UniformGridSlot, Slots)

			ARGUMENT_ATTRIBUTE(Padding, slotPadding)

			ARGUMENT_ATTRIBUTE(float, minDesiredSlotWidth)

			ARGUMENT_ATTRIBUTE(float, minDesiredSlotHeight)
		};

		void init(const BuilderArguments& arguments);

		static UniformGridSlot::SlotBuilderArguments Slot(int32_t column, int32_t row);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		template<typename SlotType>
		struct ScopedWidgetSlotArguments : public SlotType::SlotBuilderArguments
		{
			ScopedWidgetSlotArguments(std::shared_ptr<SlotType> inSlot, std::vector<std::shared_ptr<SlotType>>& inChildren, int32_t index,
				std::shared_ptr<Widget> inParentWidget)
				: SlotType::SlotBuilderArguments(inSlot)
				, m_childrens(inChildren)
				, m_index(index)
				, m_parentWidget(inParentWidget)
			{}

			virtual ~ScopedWidgetSlotArguments()
			{
				if (SlotType::SlotBuilderArguments::m_slot != nullptr)
				{
					if (m_index == -1)
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						SlotType::SlotBuilderArguments::m_slot->init(m_parentWidget, *this);
						m_childrens.push_back(std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(SlotType::SlotBuilderArguments::m_slot)));
					}
					else
					{
						//std::shared_ptr<SlotType> boxSlot = std::make_shared<SlotType>();
						//SlotType::SlotBuilderArguments slotArguments(boxSlot);
						SlotType::SlotBuilderArguments::m_slot->init(m_parentWidget, *this);
						m_childrens.insert(m_childrens.begin() + m_index, std::static_pointer_cast<SlotType>(std::static_pointer_cast<SlotBase>(SlotType::SlotBuilderArguments::m_slot)));
					}
				}
			}
		private:
			std::vector<std::shared_ptr<SlotType>>& m_childrens;

			std::shared_ptr<Widget> m_parentWidget;

			int32_t m_index;
		};

		ScopedWidgetSlotArguments<UniformGridSlot> addSlot(int32_t column, int32_t row);
	private:
		std::vector<std::shared_ptr<UniformGridSlot>> m_childrens;

		mutable int32_t m_numColumns;
		mutable int32_t m_numRows;

		Attribute<Padding> m_slotPadding;
		Attribute<float> m_minDesiredSlotWidth;
		Attribute<float> m_minDesiredSlotHeight;
	};
}