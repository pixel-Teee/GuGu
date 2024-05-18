#include <pch.h>

#include "ExpandableArea.h"
#include "NullWidget.h"
#include "TextBlockWidget.h"
#include "BoxPanel.h"
#include "Button.h"
#include "StyleSet.h"
#include "Style.h"
#include "Border.h"

namespace GuGu {
	void ExpandableArea::init(const BuilderArguments& arguments)
	{
		m_bAreaCollapsed = arguments.minitiallyCollapsed;
		m_minWidth = arguments.mminWidth;
		m_maxHeight = arguments.mmaxHeight;
		m_onAreaExpansionChanged = arguments.monAreaExpansionChanged;
		m_collapsedImage = arguments.mstyle.Get()->m_collapsedImage;
		m_expandedImage = arguments.mstyle.Get()->m_expandedImage;

		std::shared_ptr<Widget> headerContent = arguments.mHeaderContent->getChildWidget();
		if (headerContent == NullWidget::getNullWidget())
		{
			headerContent = WIDGET_NEW(TextBlockWidget)
				.text(arguments.mAreaTitle);
		}

		const bool bBodyDiffers = arguments.mbodyBorderImage.Get() != nullptr || arguments.mbodyBorderBackgroundColor.IsSet();
		const Attribute<std::shared_ptr<Brush>> fullBorderImage = bBodyDiffers ? StyleSet::getStyle()->getNoBrush() : arguments.mborderImage;
		const Attribute<math::float4> fullBorderBackgroundColor = bBodyDiffers ? math::float4(0, 0, 0, 0) : arguments.mbodyBorderBackgroundColor;
		const Attribute<std::shared_ptr<Brush>> titleBorderImage = !bBodyDiffers ? StyleSet::getStyle()->getNoBrush() : arguments.mborderImage;
		const Attribute<math::float4> m_titleBorderBackgroundColor = !bBodyDiffers ? math::float4(0, 0, 0, 0) : arguments.mborderBackgroundColor;

		m_childWidget->setChildWidget(
			WIDGET_NEW(Border)
			.brush(fullBorderImage)
			.BorderBackgroundColor(fullBorderBackgroundColor)
			.padding(0.0f)
			.Content
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Border, m_titleBorder)
					.brush(titleBorderImage)
					.BorderBackgroundColor(m_titleBorderBackgroundColor)
					.padding(0.0f)
					.Content
					(
						WIDGET_NEW(Button)
						.buttonSyle(StyleSet::getStyle()->getStyle<ButtonStyle>("NoBorder"))
						.contentPadding(arguments.mHeaderPadding)
						.Clicked(this, &ExpandableArea::onHeaderClicked)
					)
				)
				+ VerticalBox::Slot()
				.StretchHeight(1.0f)
				(
					WIDGET_NEW(Border)
					//.visibility(this, &ExpandableArea::onGetContentVisibility)
					.brush(arguments.mbodyBorderImage)
					.BorderBackgroundColor(arguments.mbodyBorderBackgroundColor)
					.padding(arguments.mpadding.Get())
					.Content
					(
						arguments.mBodyContent->m_childWidget
					)
				)
			)
		);
	}
	uint32_t ExpandableArea::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		return uint32_t();
	}
	math::float2 ExpandableArea::ComputeFixedSize(float inLayoutScaleMultiplier)
	{
		return math::float2();
	}
	void ExpandableArea::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
	{
	}
	SlotBase* ExpandableArea::getSlot(uint32_t index) const
	{
		return nullptr;
	}
	uint32_t ExpandableArea::getSlotsNumber() const
	{
		return uint32_t();
	}
	Reply ExpandableArea::onHeaderClicked()
	{

		return Reply::Handled();
	}
	void ExpandableArea::setExpandedAnimated(bool bExpanded)
	{
		const bool bShouldBeCollapsed = !bExpanded;
		if (m_bAreaCollapsed != bShouldBeCollapsed)
		{
			m_bAreaCollapsed = bShouldBeCollapsed;
			
			if (m_onAreaExpansionChanged)
			{
				m_onAreaExpansionChanged(!m_bAreaCollapsed);
			}
		}
	}
	void ExpandableArea::onToggleContentVisibility()
	{
		//set expanded animated
		setExpandedAnimated(m_bAreaCollapsed);
	}
	Visibility ExpandableArea::onGetContentVisibility() const
	{
		//float scale = 
		return Visibility::Visible;
	}
}