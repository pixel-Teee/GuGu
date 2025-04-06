#pragma once

#include "ISceneOutliner.h"
#include "SceneOutlinerFwd.h" //一些前向声明
#include "SceneOutlinerStandaloneTypes.h"//TreeItemID
#include "ObjectTreeItem.h"
#include <Core/UI/HeaderRow.h> //ColumnSortMode 
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class ITableRow;
	class HeaderRow;
	class TableViewBase;
	class ISceneOutlinerColumn;
	namespace SceneOutlinerNameSpace
	{
		class OutlinerTreeView;

		struct PendingTreeOperation //应用在 tree view 的一些操作
		{
			enum Type{ Added, Removed, Moved};

			Type m_type;

			TreeItemPtr m_item;
		};

		//存储了一个选中的 items 的集合
		struct SceneOutlinerItemSelection
		{
			mutable std::vector<std::weak_ptr<ITreeItem>> m_selectedItems;//ObjectTreeItems

			SceneOutlinerItemSelection() {}

			SceneOutlinerItemSelection(const std::vector<TreeItemPtr>& inSelectedItems);

			SceneOutlinerItemSelection(OutlinerTreeView& tree);

			template<typename DataType>
			std::vector<DataType> getData() const
			{
				std::vector<DataType> result;
				for (std::weak_ptr<ITreeItem>& item : m_selectedItems)
				{
					if (item.lock())
					{
						std::shared_ptr<ObjectTreeItem> objectTreeItem = std::static_pointer_cast<ObjectTreeItem>(item.lock());//todo:这里以后要修复
						//DataType data;
						result.push_back(static_cast<DataType>(objectTreeItem->m_gameObject.lock().get()));
					}	
				}
				return result;
			}
		};

		class SceneOutliner : public ISceneOutliner
		{
		public:
			SceneOutliner() {}

			virtual ~SceneOutliner() {}

			struct BuilderArguments : public Arguments<SceneOutliner>
			{
				BuilderArguments() {}

				~BuilderArguments() = default;
			};

			void init(const BuilderArguments& arguments);

			Visibility getEmptyLabelVisibility() const;

		public:
			const std::map<GuGuUtf8Str, std::shared_ptr<ISceneOutlinerColumn>>& getColumns() const
			{
				return m_columns;
			}

			//填充 m_rootTreeItems
			void populate();

			//构造一个新的item，对于确切的类型，并且刷新树
			template<typename TreeItemType, typename DataType>
			void constructItemFor(const DataType& data)
			{
				const TreeItemType temporary(data);

				TreeItemPtr newItem = std::make_shared<TreeItemType>(data);
				m_pendingOperations.push_back({PendingTreeOperation::Added, newItem});

				refresh();
			}

			virtual void refresh() override;

			virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

			void onLevelChanged();
		private:
			void onOutlinerTreeDoubleClick(TreeItemPtr treeItem);

			void onOutlinerTreeSelectionChanged(TreeItemPtr treeItem, SelectInfo::Type selectInfo);

			//重新填充整棵树
			void rePopulateEntireTree();

			bool addItemToTree(TreeItemPtr inItem);

			void addUnfilteredItemToTree(TreeItemPtr item);

			TreeItemPtr ensureParentForItem(TreeItemPtr item);

			void emptyTreeItems();

			void setupColumns(HeaderRow& headerRow);

			//ISceneOutlinerColumn描述了标头和每行怎么构造 widget
			//key描述了列的名字
			std::map<GuGuUtf8Str, std::shared_ptr<ISceneOutlinerColumn>> m_columns;

			std::shared_ptr<ITableRow> onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable);

			void onGetChildrenForOutlinerTree(TreeItemPtr inParent, std::vector<TreeItemPtr>& outChildren);

			//scene outliner 的 header row
			std::shared_ptr<HeaderRow> m_headerRowWidget;

			//根层级的 tree items
			std::vector<TreeItemPtr> m_rootTreeItems;

			std::shared_ptr<OutlinerTreeView> m_outlinerTreeView;

			//应用到树上的一些操作，每次最多应用500次
			std::vector<PendingTreeOperation> m_pendingOperations;

			//刷新列数据，比如增加一列，或者减少一列，改变列的属性，即 header row widget
			uint8_t m_bNeedsColumnRefresh : 1;

			//完整的刷新
			bool m_bFullRefresh : 1;

			bool m_bSortDirty : 1;

			//根据哪一列去排序
			GuGuUtf8Str m_sortByName;
			//当前选择的排序方式
			ColumnSortMode::Type m_sortMode;

			TreeItemMap m_treeItemMap;//实际的对象的ID，比如GameObject的名字对应的TreeItem，方便索引
		};
	}
}