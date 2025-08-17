#include <pch.h>

#include "ObjectTreeItem.h"
#include "ObjectTreeLabel.h"//UI
#include "SceneOutlinerStandaloneTypes.h"//SceneOutliner::TreeItemID

namespace GuGu {
	ObjectTreeItem::ObjectTreeItem(std::shared_ptr<GameObject> inObject)
		: m_gameObject(inObject)
	{
		m_gameObjectLabel = inObject->getGameObjectName();
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

	void ObjectTreeItem::setDisplayString(const GuGuUtf8Str& inName)
	{
		m_gameObjectLabel = inName;
	}

	SceneOutlinerNameSpace::TreeItemPtr ObjectTreeItem::findParent(const SceneOutlinerNameSpace::TreeItemMap& existingItems) const
	{
		std::shared_ptr<GameObject> gameObject = m_gameObject.lock();
		if(!gameObject)
			return nullptr;

		std::shared_ptr<GameObject> parentGameObject = gameObject->getParentGameObject().lock();
		if (parentGameObject)
		{
			SceneOutlinerNameSpace::TreeItemID id(parentGameObject);
			auto it = existingItems.find(id);
			if (it != existingItems.end())
				return it->second;
			return nullptr;
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
		return m_gameObject.lock();
	}

	void ObjectTreeItem::addChild(SceneOutlinerNameSpace::TreeItemPtr child)
	{
		std::shared_ptr<ObjectTreeItem> objectTreeItem = std::static_pointer_cast<ObjectTreeItem>(child);
		//find index
		std::shared_ptr<GameObject> gameObject = m_gameObject.lock();
		int32_t index = gameObject->findIndex(objectTreeItem->m_gameObject.lock());

		if (index != -1)
		{
			child->m_parent = shared_from_this();	
			m_children.push_back(std::move(child));

			std::vector<std::weak_ptr<ITreeItem>> copyedChildren = m_children;

			//sort children
			m_children.clear();
			Array<std::shared_ptr<GameObject>>& childrens = gameObject->getChildrens();
			for (int32_t i = 0; i < childrens.size(); ++i)
			{
				auto it = std::find_if(copyedChildren.begin(), copyedChildren.end(), [&](std::weak_ptr<ITreeItem>& findItem) {
					if (findItem.lock())
					{
						std::shared_ptr<ObjectTreeItem> lockedObjectTreeItem = std::static_pointer_cast<ObjectTreeItem>(findItem.lock());
						if (lockedObjectTreeItem)
						{
							std::shared_ptr<GameObject> lockedGameObject = lockedObjectTreeItem->m_gameObject.lock();
							return lockedGameObject == childrens[i];
						}
						else
						{
							GuGu_LOGE("object tree item fatal error to sort");
							return false;
						}
					}
					else
					{
						GuGu_LOGE("object tree item fatal error to sort");
						return false;
					}
				});

				if(it != copyedChildren.end())
					m_children.push_back(*it);
			}
		}
		else
		{
			GuGu_LOGE("object tree item fatal error to find index");
		}
	}

}