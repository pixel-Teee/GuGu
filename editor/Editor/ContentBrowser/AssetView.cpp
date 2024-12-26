#include <pch.h>

#include "AssetView.h"

#include <Core/UI/BoxPanel.h>
#include <Core/UI/Border.h>
#include <Core/UI/NullWidget.h>

#include "AssetViewWidgets.h"

namespace GuGu {
	void AssetView::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		//m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		m_currentViewType = AssetViewType::Tile;
		//create path view
		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox)	
		+ VerticalBox::Slot()
		(
			//显示资产文件和文件夹的 tile view 挂在这个容器下
			WIDGET_ASSIGN_NEW(Border, m_viewContainer)
		);
		createCurrentView();

		m_childWidget->m_childWidget = verticalBox;
	}
	AssetView::~AssetView()
	{

	}
	void AssetView::createCurrentView()
	{
		m_tileView.reset();

		std::shared_ptr<Widget> newView = NullWidget::getNullWidget();
		switch (m_currentViewType)
		{
		case AssetViewType::Tile:
			m_tileView = createTileView();
		default:
			break;
		}
		m_viewContainer->setContent(newView);
	}
	std::shared_ptr<AssetTileView> AssetView::createTileView()
	{
		return WIDGET_NEW(AssetTileView);
	}
}