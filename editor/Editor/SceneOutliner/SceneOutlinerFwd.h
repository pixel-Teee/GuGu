#pragma once

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		struct TreeItemID;
		struct ITreeItem;
		typedef std::shared_ptr<ITreeItem> TreeItemPtr;

		typedef std::map<TreeItemID, TreeItemPtr> TreeItemMap;

		class SceneOutliner;
	}
	using OnSceneOutlinerItemPicker = std::function<void(std::shared_ptr<SceneOutlinerNameSpace::ITreeItem>)>;
}