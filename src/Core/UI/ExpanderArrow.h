#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "Slot.h"

namespace GuGu {
	class Button;
	class ITableRow;
	//展开箭头
	class ExpanderArrow : public Widget
	{
	public:
		ExpanderArrow();

		virtual ~ExpanderArrow();

		struct BuilderArguments : public Arguments<ExpanderArrow>
		{
			BuilderArguments()
				: mindentAmount(10)
				, mbaseIndentLevel(0)
				, mshouldDrawWires(false)
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(float, indentAmount)

			ARGUMENT_ATTRIBUTE(int32_t, baseIndentLevel)

			ARGUMENT_ATTRIBUTE(bool, shouldDrawWires)
		};

		void init(const BuilderArguments& arguments, const std::shared_ptr<ITableRow>& tableRow);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		Padding getExpanderPadding() const;
	protected:
		std::shared_ptr<SingleChildSlot> m_childWidget;

		std::weak_ptr<ITableRow> m_ownerRowPtr;

		//展开按钮
		std::shared_ptr<Button> m_expanderArrow;

		//缩进的深度
		Attribute<float> m_indentAmount;

		//树开始的缩进深度
		Attribute<int32_t> m_baseIndentLevel;

		//是否绘制线条
		Attribute<bool> m_shouldDrawWires;
	};
}