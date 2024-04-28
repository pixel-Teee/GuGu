#pragma once

#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	class NullWidget : public Widget
	{
	public:
		NullWidget();

		virtual ~NullWidget();

		struct BuilderArguments : public Arguments<NullWidget>
		{
			BuilderArguments() = default;
			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		static std::shared_ptr<NullWidget> nullWidget;

		static std::shared_ptr<NullWidget> getNullWidget();
	private:
	};

	
}