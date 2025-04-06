#include <pch.h>

#include "DetailExpanderArrow.h"
#include <Core/UI/Box.h>
#include <Core/UI/Attribute.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Button.h>
#include "DetailTableRowBase.h"

namespace GuGu {

	void DetailExpanderArrow::init(const BuilderArguments& arguments, std::shared_ptr<DetailTableRowBase> detailsRow)
	{
		m_row = detailsRow;

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = WIDGET_NEW(BoxWidget)
									   .visibility(Attribute<Visibility>::CreateSP(this, &DetailExpanderArrow::getExpanderVisibility))
									   .VAlign(VerticalAlignment::Center)
									   .HAlign(HorizontalAlignment::Center)
									   .Content
									   (
										   WIDGET_ASSIGN_NEW(Button, m_expanderArrow)
										   .verticalAlignment(VerticalAlignment::Center)
										   .horizontalAlignment(HorizontalAlignment::Center)
										   .Clicked(this, &DetailExpanderArrow::onExpanderClicked)
										   .buttonSyle(CoreStyle::getStyleSet()->getStyle<ButtonStyle>("NoBorder"))
										   .Content
										   (
												WIDGET_NEW(ImageWidget)
											    .brush(this, &DetailExpanderArrow::getExpanderImage)
										   )
									   );
	}

	Visibility DetailExpanderArrow::getExpanderVisibility() const
	{
		std::shared_ptr<DetailTableRowBase> rowPtr = m_row.lock();
		if (!rowPtr)
		{
			return Visibility::Collapsed;
		}
		return rowPtr->doesItemHaveChildren() ? Visibility::Visible : Visibility::Collapsed;
	}

	std::shared_ptr<Brush> DetailExpanderArrow::getExpanderImage() const
	{
		const bool bIsItemExpanded = m_row.lock()->isItemExpanded();

		GuGuUtf8Str resourceName;
		if (bIsItemExpanded)
		{
			resourceName = "expandedArrow";
		}
		else
		{
			resourceName = "expandedArrowCollapsed";
		}

		return CoreStyle::getStyleSet()->getBrush(resourceName);
	}

	Reply DetailExpanderArrow::onExpanderClicked()
	{
		std::shared_ptr<DetailTableRowBase> rowPtr = m_row.lock();
		if (!rowPtr)
		{
			return Reply::Unhandled();
		}

		rowPtr->toggleExpansion();

		return Reply::Handled();
	}

}