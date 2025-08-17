#pragma once

#include "ITreeItem.h"

#include <Core/GamePlay/GameObject.h>
#include <Core/UI/ListView.h>
#include <Core/UI/TableRow.h>

namespace GuGu {
	struct ObjectTreeItem : public SceneOutlinerNameSpace::ITreeItem
	{
	public:

		ObjectTreeItem(std::shared_ptr<GameObject> inObject);

		virtual ~ObjectTreeItem() {}

		virtual std::shared_ptr<Widget> generateLabelWidget(ISceneOutliner& outliner, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& inRow) override;

		virtual GuGuUtf8Str getDisplayString() const override;

		virtual SceneOutlinerNameSpace::TreeItemPtr findParent(const SceneOutlinerNameSpace::TreeItemMap& existingItems) const override;

		virtual SceneOutlinerNameSpace::TreeItemPtr createParent() const override;

		virtual SceneOutlinerNameSpace::TreeItemID getID() const override;

		virtual void addChild(SceneOutlinerNameSpace::TreeItemPtr child) override;

		mutable std::weak_ptr<GameObject> m_gameObject;

		GuGuUtf8Str m_gameObjectLabel;
	};
	
}