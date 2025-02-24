#include <pch.h>

#include "SceneOutlinerItemLabelColumn.h"
#include "SceneOutlinerTypes.h"
#include "ITreeItem.h"

namespace GuGu {

	const std::shared_ptr<GuGu::Widget> SceneOutlinerItemLabelColumn::constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row)
	{
		ISceneOutliner* outliner = m_weakSceneOutliner.lock().get();
		assert(outliner != nullptr);
		return treeItem->generateLabelWidget(*outliner, row);
	}

	HeaderRow::GColumn::BuilderArguments SceneOutlinerItemLabelColumn::constructHeaderRowColumn()
	{
		return HeaderRow::Column(getColumnID())
			   .FillWidth(5.0f)
			   .headerContent//no header content
			   (
				   NullWidget::getNullWidget()
			   );
	}

	GuGuUtf8Str SceneOutlinerItemLabelColumn::getColumnID()
	{
		return SceneOutlinerNameSpace::BuiltInColumnTypes::Label();
	}

}