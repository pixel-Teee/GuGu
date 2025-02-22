#pragma once

#include "SceneOutlinerFwd.h"
#include <Core/UI/HeaderRow.h>

namespace GuGu {
	class Widget;
	template<typename ItemType> class TableRow;
	class ISceneOutlinerColumn : public std::enable_shared_from_this<ISceneOutlinerColumn>
	{
	public:
		virtual ~ISceneOutlinerColumn(){}

		virtual const std::shared_ptr<Widget> constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, 
		const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row) = 0;

		virtual HeaderRow::GColumn::BuilderArguments constructHeaderRowColumn() = 0;

		virtual GuGuUtf8Str getColumnID() = 0;
	};
}