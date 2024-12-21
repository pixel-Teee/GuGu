#include <pch.h>

#include "SourcesViewWidgets.h"

#include <Core/UI/Border.h>

namespace GuGu {
	void AssetTreeItem::init(const BuilderArguments& arguments)
	{
		m_treeItem = arguments.mtreeItem;

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		m_childWidget->setChildWidget
		(
			WIDGET_NEW(Border)

		);
	}
	AssetTreeItem::~AssetTreeItem()
	{
	}
}