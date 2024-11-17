#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	//菜单内容会显示在其他东西纸上
	class Popup : public Widget
	{
	public:
		Popup();

		virtual ~Popup();

		struct BuilderArguments : public Arguments<Popup>
		{
			BuilderArguments()
			{
				mVisibility = Visibility::SelfHitTestInvisible;
			}

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;
	protected:
		std::shared_ptr<SingleChildSlot> m_childWidget;
	};
}