#pragma once

#include "ISceneOutlinerColumn.h"
#include "SceneOutlinerTypes.h"
#include <Core/UI/HeaderRow.h> //header row

namespace GuGu {
	class ISceneOutliner;
	namespace SceneOutlinerNameSpace
	{
		//显示 actor 的具体信息，用于 SceneOutliner，带有眼睛图标的 visibility
		class SceneOutlinerGutter : public ISceneOutlinerColumn
		{
		public:
			SceneOutlinerGutter(ISceneOutliner& outliner);

			virtual ~SceneOutlinerGutter() {}

			static GuGuUtf8Str getID() { return BuiltInColumnTypes::Gutter(); }

			const std::shared_ptr<Widget> constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row) override;

			virtual HeaderRow::GColumn::BuilderArguments constructHeaderRowColumn() override;

			virtual GuGuUtf8Str getColumnID() override { return getID(); }
		};
	}
	
}