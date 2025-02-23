#pragma once

#include "ISceneOutliner.h" //ISceneOutliner
#include "ISceneOutlinerColumn.h"

namespace GuGu {
	class ISceneOutliner;
	class SceneOutlinerItemLabelColumn : public ISceneOutlinerColumn
	{
	public:
		SceneOutlinerItemLabelColumn(ISceneOutliner& sceneOutliner)
			: m_weakSceneOutliner(std::static_pointer_cast<ISceneOutliner>(sceneOutliner.shared_from_this()))
		{}

		const std::shared_ptr<Widget> constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row) override;

		HeaderRow::GColumn::BuilderArguments constructHeaderRowColumn() override;

		GuGuUtf8Str getColumnID() override;
	private:
		std::weak_ptr<ISceneOutliner> m_weakSceneOutliner;
	};
}