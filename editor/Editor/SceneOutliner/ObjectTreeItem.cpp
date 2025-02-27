#include <pch.h>

#include "ObjectTreeItem.h"
#include "ObjectTreeLabel.h"//UI
#include "SceneOutlinerStandaloneTypes.h"//SceneOutliner::TreeItemID

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

	SceneOutlinerNameSpace::TreeItemPtr ObjectTreeItem::findParent(const SceneOutlinerNameSpace::TreeItemMap& existingItems) const
	{
		std::shared_ptr<GameObject> gameObject = m_gameObject.lock();
		if(!gameObject)
			return nullptr;

		std::shared_ptr<GameObject> parentGameObject = gameObject->getParentGameObject().lock();
		if (parentGameObject)
		{
			return existingItems.find(SceneOutlinerNameSpace::TreeItemID(parentGameObject))->second;
		}
		else
		{
			return nullptr;//todo:show folders
		}

		return nullptr;
	}

	SceneOutlinerNameSpace::TreeItemPtr ObjectTreeItem::createParent() const
	{
		std::shared_ptr<GameObject> gameObject = m_gameObject.lock();
		if (!gameObject)
			return nullptr;

		std::shared_ptr<GameObject> parentGameObject = gameObject->getParentGameObject().lock();
		if (parentGameObject && (parentGameObject != gameObject))
		{
			return std::make_shared<ObjectTreeItem>(parentGameObject);
		}
		else if (!parentGameObject)
		{
			return nullptr;//todo:fix this
		}

		return nullptr;
	}

	SceneOutlinerNameSpace::TreeItemID ObjectTreeItem::getID() const
	{
		return m_gameObjectLabel;
	}
}