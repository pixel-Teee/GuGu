#include <pch.h>

#include "ObjectTreeItem.h"
#include "ObjectTreeLabel.h"//UI

namespace GuGu {

	ObjectTreeItem::ObjectTreeItem(std::shared_ptr<GameObject> inObject)
		: m_gameObject(inObject)
	{
		m_gameObjectLabel = inObject->getGameObjectLabel();
	}

	std::shared_ptr<Widget> ObjectTreeItem::generateLabelWidget(ISceneOutliner& outliner, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& inRow)
	{
		return WIDGET_NEW(SceneOutlinerNameSpace::ObjectTreeLabel, *this, outliner, inRow);
		//return NullWidget::getNullWidget();
	}

	GuGuUtf8Str ObjectTreeItem::getDisplayString() const
	{
		return m_gameObjectLabel;
	}

}