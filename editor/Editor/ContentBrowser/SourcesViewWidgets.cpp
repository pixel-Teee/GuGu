#include <pch.h>

#include "SourcesViewWidgets.h"
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/UI/Border.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/TextBlockWidget.h>

namespace GuGu {
	void AssetTreeItem::init(const BuilderArguments& arguments)
	{
		m_treeItem = arguments.mtreeItem;
		m_isItemExpanded = arguments.misItemExpanded;
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		m_childWidget->setChildWidget
		(
			//WIDGET_NEW(Border)
			//.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige4"))
			//.Content
			//(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.FixedWidth()
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					//folder icon
					WIDGET_NEW(ImageWidget)
					.brush(this, &AssetTreeItem::getFolderIcon)
				)
				+ HorizontalBox::Slot()
				.FixedWidth()
				.setVerticalAlignment(VerticalAlignment::Center)
				.setPadding(Padding(5, 0, 0, 0))
				(
					WIDGET_NEW(TextBlockWidget)
					.text(this, &AssetTreeItem::getNameText)
					.textColor(EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9"))
				)
			//)
		);

		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		m_folderOpenBrush = EditorStyleSet::getStyleSet()->getBrush("OpenFolder");
		m_folderClosedBrush = EditorStyleSet::getStyleSet()->getBrush("CloseFolder");
	}
	AssetTreeItem::~AssetTreeItem()
	{
	}

	std::shared_ptr<GuGu::Brush> AssetTreeItem::getFolderIcon() const
	{
		//return EditorStyleSet::getStyleSet()->getBrush("CloseFolder");
		return (m_isItemExpanded.Get()) ? EditorStyleSet::getStyleSet()->getBrush("OpenFolder") : EditorStyleSet::getStyleSet()->getBrush("CloseFolder");
	}

	GuGuUtf8Str AssetTreeItem::getNameText() const
	{
		std::shared_ptr<TreeItem> treeItemLock = m_treeItem.lock();
		if (treeItemLock)
		{
			return treeItemLock->m_folderName;
		}
		else
		{
			return GuGuUtf8Str("");
		}
	}

}