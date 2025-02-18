#pragma once

#include "Widget.h"

#include "Slot.h"
#include "UIMacros.h"
#include "BasicElement.h"

namespace GuGu {
	class BoxPanel : public Widget
	{
	public:
		BoxPanel();

		BoxPanel(Orientation orientation);

		virtual ~BoxPanel();

		struct BuilderArguments : public Arguments<BoxPanel>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			//ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)
		};

		template<typename SlotType>
		class BoxSlot : public Slot<SlotType>
		{
		public:

			struct SlotBuilderArguments : public Slot<SlotType>::SlotBuilderArguments
			{
				SlotBuilderArguments(std::shared_ptr<SlotType> inSlot)
					: Slot<SlotType>::SlotBuilderArguments(inSlot){}
				virtual ~SlotBuilderArguments() {};

				//ARGUMENT_ATTRIBUTE(std::optional<SizeParam>, sizeParam)
				std::optional<SizeParam> m_sizeParam;
				Attribute<float> MaxSize;
			};
		public:
			BoxSlot()
				: Slot<SlotType>(HorizontalAlignment::Stretch, VerticalAlignment::Stretch, Padding(0.0f, 0.0f, 0.0f, 0.0f))
				, m_sizeParam(SizeParam::Stretch, 1.0f)
				, m_maxSize(0.0f)
			{}

			virtual ~BoxSlot() = default;

			SizeParam::SizeRule getSizeRule() const
			{
				return m_sizeParam.m_sizeRule;
			}

			float getSizeValue() const
			{
				return m_sizeParam.m_value.Get();
			}

			float getMaxSize() const
			{
				return m_maxSize;
			}

			SizeParam m_sizeParam;

			float m_maxSize;//0.0 is not specified
		};

		class BoxPanelSlot : public BoxSlot<BoxPanelSlot>
		{

		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

	protected:

		const Orientation m_orientation;

		std::vector<std::shared_ptr<BoxPanelSlot>> m_childrens;
	};

	class HorizontalBox : public BoxPanel
	{
	public:
		HorizontalBox();

		virtual ~HorizontalBox();

		class HorizontalBoxSlot : public BoxPanel::BoxSlot<HorizontalBoxSlot>
		{
		public:
			HorizontalBoxSlot() {}

			virtual ~HorizontalBoxSlot() {}

			void init(std::shared_ptr<Widget> inParentWidget, const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);
				m_maxSize = builderArguments.MaxSize.Get();
				m_sizeParam = builderArguments.m_sizeParam.value();
			}

			struct SlotBuilderArguments : public BoxPanel::BoxSlot<HorizontalBoxSlot>::SlotBuilderArguments
			{
				//using BoxPanel::BoxSlot::SlotBuilderArguments;
				SlotBuilderArguments(std::shared_ptr<HorizontalBoxSlot> inSlot)
					: BoxPanel::BoxSlot<HorizontalBoxSlot>::SlotBuilderArguments(inSlot)
				{}

				virtual ~SlotBuilderArguments() {}

				SlotBuilderArguments& FixedWidth()
				{
					m_sizeParam = Fixed();
					return Me();
				}

				SlotBuilderArguments& StretchWidth(Attribute<float> inStretchCoefficient)
				{
					m_sizeParam = Stretch(inStretchCoefficient);
					return Me();
				}
			};	
		private:
		};

		static HorizontalBoxSlot::SlotBuilderArguments Slot()
		{
			return HorizontalBoxSlot::SlotBuilderArguments(std::make_shared<HorizontalBoxSlot>());
		}

		HorizontalBoxSlot::SlotBuilderArguments addSlot()
		{
			return insertSlot(-1);
		}

		HorizontalBoxSlot::SlotBuilderArguments insertSlot(int32_t index = -1)
		{
			if (index == -1)
			{
				std::shared_ptr<HorizontalBoxSlot> boxSlot = std::make_shared<HorizontalBoxSlot>();
				HorizontalBoxSlot::SlotBuilderArguments slotArguments(boxSlot);
				boxSlot->init(shared_from_this(), slotArguments);
				m_childrens.push_back(std::static_pointer_cast<BoxPanelSlot>(std::static_pointer_cast<SlotBase>(boxSlot)));
				return slotArguments;
			}
			else
			{
				std::shared_ptr<HorizontalBoxSlot> boxSlot = std::make_shared<HorizontalBoxSlot>();
				HorizontalBoxSlot::SlotBuilderArguments slotArguments(boxSlot);
				boxSlot->init(shared_from_this(), slotArguments);
				m_childrens.insert(m_childrens.begin() + index, std::static_pointer_cast<BoxPanelSlot>(std::static_pointer_cast<SlotBase>(boxSlot)));
				return slotArguments;
			}
		}

		struct BuilderArguments : public Arguments<HorizontalBox>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(HorizontalBoxSlot, Slots)
		};

		void init(const BuilderArguments& arguments);	
	};

	class VerticalBox : public BoxPanel
	{
	public:
		VerticalBox();

		virtual ~VerticalBox();

		class VerticalBoxSlot : public BoxPanel::BoxSlot<VerticalBoxSlot>
		{
		public:
			VerticalBoxSlot() {}

			virtual ~VerticalBoxSlot() {}

			void init(std::shared_ptr<Widget> inParentWidget, const SlotBuilderArguments& builderArguments)
			{
				m_parentWidget = inParentWidget;
				m_childWidget->setParentWidget(inParentWidget);
				m_maxSize = builderArguments.MaxSize.Get();
				m_sizeParam = builderArguments.m_sizeParam.value();
			}

			struct SlotBuilderArguments : public BoxPanel::BoxSlot<VerticalBoxSlot>::SlotBuilderArguments
			{
				//using BoxPanel::BoxSlot::SlotBuilderArguments;
				SlotBuilderArguments(std::shared_ptr<VerticalBoxSlot> inSlot)
					: BoxPanel::BoxSlot<VerticalBoxSlot>::SlotBuilderArguments(inSlot)
				{}

				virtual ~SlotBuilderArguments() {}

				SlotBuilderArguments& FixedHeight()
				{
					m_sizeParam = Fixed();
					return Me();
				}

				SlotBuilderArguments& StretchHeight(Attribute<float> inStretchCoefficient)
				{
					m_sizeParam = Stretch(inStretchCoefficient);
					return Me();
				}
			};
		private:
		};

		static VerticalBoxSlot::SlotBuilderArguments Slot()
		{
			return VerticalBoxSlot::SlotBuilderArguments(std::make_shared<VerticalBoxSlot>());
		}

		VerticalBox::VerticalBoxSlot::SlotBuilderArguments insertSlot(int32_t insertAtIndex);

		struct BuilderArguments : public Arguments<VerticalBox>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_SLOT(VerticalBoxSlot, Slots)
		};

		void init(const BuilderArguments& arguments);
	};
}