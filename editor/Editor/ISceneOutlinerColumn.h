#pragma once

#include "SceneOutlinerFwd.h"

namespace GuGu {
	class Widget;
	template<typename ItemType> class TableRow;
	class ISceneOutlinerColumn : public std::enable_shared_from_this<ISceneOutlinerColumn>
	{
	public:
		virtual ~ISceneOutlinerColumn(){}

		virtual const std::shared_ptr<Widget> constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, 
		const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row) = 0;
	};
}