#pragma once

#include "Widget.h"

#include "Slot.h"
#include "BasicElement.h"

namespace GuGu {
	class BoxPanel : public Widget
	{
	public:
		BoxPanel();

		virtual ~BoxPanel();

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			//ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)
		};

		class BoxSlot : public Slot
		{
		public:
			BoxSlot()
				: Slot(HorizontalAlignment::Stretch, VerticalAlignment::Stretch)
				, m_sizeParam(SizeParam::Stretch, 1.0f)
				, m_maxSize(0.0f)
			{}

			virtual ~BoxSlot() = default;

			SizeParam m_sizeParam;

			float m_maxSize;
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

	private:

		const Orientation m_orientation;
	};
}