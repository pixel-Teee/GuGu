#pragma once

#include "ISceneOutliner.h"
#include "SceneOutlinerFwd.h" //一些前向声明
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class ITableRow;
	class HeaderRow;
	class TableViewBase;
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
		private:

			std::shared_ptr<ITableRow> onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable);

			//scene outliner 的 header row
			std::shared_ptr<HeaderRow> m_headerRowWidget;

			std::vector<TreeItemPtr> m_rootTreeItems;

			std::shared_ptr<OutlinerTreeView> m_outlinerTreeView;
		};
	}
}