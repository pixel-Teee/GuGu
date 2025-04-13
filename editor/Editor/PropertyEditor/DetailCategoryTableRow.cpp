#include <pch.h>

#include "DetailCategoryTableRow.h"
#include "DetailExpanderArrow.h"
#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	void DetailCategoryTableRow::init(const BuilderArguments& inArgs, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView)
	{
		m_ownerTreeNode = inOwnerTreeNode;

		std::shared_ptr<HorizontalBox> headerBox = WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.setHorizontalAlignment(HorizontalAlignment::Left)
			.setVerticalAlignment(VerticalAlignment::Center)
			.setPadding(Padding(2, 0, 0, 0))
			.FixedWidth()
			(
				WIDGET_NEW(DetailExpanderArrow, std::static_pointer_cast<DetailCategoryTableRow>(shared_from_this()))
			)
			+ HorizontalBox::Slot()
			.setVerticalAlignment(VerticalAlignment::Center)
			.setPadding(Padding(4, 0, 0, 0))
			.FixedWidth()
			(
				WIDGET_NEW(TextBlockWidget)
				.text(inArgs.mdisplayName)
				.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
			);

		if (inArgs.mheaderContent)
		{
			headerBox->addSlot()
			.setVerticalAlignment(VerticalAlignment::Center)
			.StretchWidth(1.0f)
			(
				inArgs.mheaderContent
			);
		}

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = headerBox;

		TableRow<std::shared_ptr<DetailTreeNode>>::initInternal(
			TableRow::BuilderArguments()
			.Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.beige"))
			.Content
			(
				NullWidget::getNullWidget()
			),
			inOwnerTableView
		);
	}

}