#include <pch.h>

#include "DetailsView.h"
#include <Core/UI/NullWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>

namespace GuGu {

	void DetailsView::init(const BuilderArguments& arguments)
	{
		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

		verticalBox->addSlot()
		.StretchHeight(1)
		.setPadding(Padding(0, 0, 0, 0))
		(
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				constructTreeView()
			)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = verticalBox;
	}

	DetailsView::~DetailsView()
	{

	}

	std::shared_ptr<DetailTree> DetailsView::constructTreeView()
	{
		return WIDGET_ASSIGN_NEW(DetailTree, m_detailTree)
			   .treeItemSource(&m_rootTreeNodes)
			   .onGenerateRow(this, &DetailsView::onGenerateRowForDetailTree)
			   .onGetChildren(this, &DetailsView::onGetChildrenForDetailTree);
	}

}