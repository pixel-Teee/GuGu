#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

#include <Renderer/nvrhi.h>

namespace GuGu {
	class ViewportWidget : public Widget
	{
	public:
		ViewportWidget();

		virtual ~ViewportWidget();

		struct BuilderArguments : public Arguments<ViewportWidget>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index)  const override;

		virtual uint32_t getSlotsNumber() const override;

		void setRenderTarget(nvrhi::TextureHandle renderTarget);
	protected:
		std::shared_ptr<SingleChildSlot> m_childWidget;

		nvrhi::TextureHandle m_renderTarget;
	};
}