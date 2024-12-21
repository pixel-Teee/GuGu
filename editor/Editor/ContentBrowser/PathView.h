#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

#include <Core/UI/TreeView.h>

#include "ContentBrowserDelegates.h"

namespace GuGu {
	class TreeItem;
	//文件夹的视图
	class PathView : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PathView>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		~PathView();

		void getChildrenForTree(std::shared_ptr<TreeItem> treeItem, std::vector<std::shared_ptr<TreeItem>>& outChildren);

		//处理树形视图选中的逻辑
		void treeSelectionChanged(std::shared_ptr<TreeItem> treeItem, SelectInfo::Type selectInfo);

		//处理树形视图展开的逻辑
		void treeExpansionChanged(std::shared_ptr<TreeItem> treeItem, bool bIsExpanded);

		virtual std::shared_ptr<ITableRow> generateTreeRow(std::shared_ptr<TreeItem> treeItem, const std::shared_ptr<TableViewBase>& ownerTable);
	private:
		//上一次被 on path selected 事件通知的路径，用于保留选择，当过滤文件夹的时候
		std::set<GuGuUtf8Str> m_lastSelectedPaths;

		//tree view widget
		std::shared_ptr<TreeView<std::shared_ptr<TreeItem>>> m_treeViewPtr;

		//树中文件夹的列表
		std::vector<std::shared_ptr<TreeItem>> m_treeRootItems;

		OnContentBrowserItemSelectionChanged m_itemSelectionChanged;
	};
}