#include <pch.h>

#include  "DetailSingleItemRow.h"
#include  "DetailCategoryBuilderImpl.h"//DetailLayoutCustomization
#include  <Core/UI/Box.h>//box widget

namespace GuGu {

	void DetailSingleItemRow::init(const BuilderArguments& inArgs, DetailLayoutCustomization* inCustomization, bool bHasMultipleColumns, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView)
	{
		m_ownerTreeNode = inOwnerTreeNode;
		m_customization = inCustomization;

		std::shared_ptr<Widget> widget = NullWidget::getNullWidget();
		const bool bIsValidTreeNode = true;

		//const bool bIsValidTreeNode = true;
		if (bIsValidTreeNode)
		{
			if (m_customization->isValidCustomization())
			{
				m_widgetRow = m_customization->getWidgetRow();

				std::shared_ptr<Splitter> splitter = WIDGET_NEW(Splitter);

				widget = WIDGET_NEW(Border)
						 .padding(0)
						 .Content
						 (
							 splitter
						 );

				std::shared_ptr<Widget> nameWidget = m_widgetRow.m_nameWidget.m_widget;
				widget = nameWidget;
			}
		}

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = WIDGET_NEW(BoxWidget)
		.MinDesiredHeight(26.0f)//最小高度
		.Content
		(
			WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.setVerticalAlignment(VerticalAlignment::Stretch)
			.setHorizontalAlignment(HorizontalAlignment::Stretch)
			.FixedWidth()
			(
				WIDGET_NEW(Border)
				.padding(0)
				.Content
				(
					widget
				)
			)
		);

		TableRow<std::shared_ptr<DetailTreeNode>>::initInternal(
			TableRow::BuilderArguments()
			.Content
			(
				NullWidget::getNullWidget()
			),
			inOwnerTableView
		);
	}

}