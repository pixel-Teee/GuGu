#pragma once

#include "ISceneOutliner.h"
#include "SceneOutlinerFwd.h" //一些前向声明
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class ITableRow;
	class HeaderRow;
	class TableViewBase;
	class ISceneOutlinerColumn;
	namespace SceneOutlinerNameSpace
	{
		class OutlinerTreeView;

		struct PendingTreeOperation
		{
			enum Type{ Added, Removed, Moved};

			Type m_type;

			TreeItemPtr m_item;
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
			//重新填充整棵树
			void rePopulateEntireTree();

			bool addItemToTree(TreeItemPtr inItem);

			void addUnfilteredItemToTree(TreeItemPtr item);

			void emptyTreeItems();

			void setupColumns(HeaderRow& headerRow);

			std::map<GuGuUtf8Str, std::shared_ptr<ISceneOutlinerColumn>> m_columns;

			std::shared_ptr<ITableRow> onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable);

			void onGetChildrenForOutlinerTree(TreeItemPtr inParent, std::vector<TreeItemPtr>& outChildren);

			//scene outliner 的 header row
			std::shared_ptr<HeaderRow> m_headerRowWidget;

			std::vector<TreeItemPtr> m_rootTreeItems;

			std::shared_ptr<OutlinerTreeView> m_outlinerTreeView;

			std::vector<PendingTreeOperation> m_pendingOperations;

			uint8_t m_bNeedsColumnRefresh : 1;

			bool m_bFullRefresh : 1;

			bool m_bSortDirty : 1;
		};
	}
}