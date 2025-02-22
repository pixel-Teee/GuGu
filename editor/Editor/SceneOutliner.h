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
		private:
			void setupColumns(HeaderRow& headerRow);

			std::map<GuGuUtf8Str, std::shared_ptr<ISceneOutlinerColumn>> m_columns;

			std::shared_ptr<ITableRow> onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable);

			//scene outliner 的 header row
			std::shared_ptr<HeaderRow> m_headerRowWidget;

			std::vector<TreeItemPtr> m_rootTreeItems;

			std::shared_ptr<OutlinerTreeView> m_outlinerTreeView;

			uint8_t m_bNeedsColumnRefresh : 1;
		};
	}
}