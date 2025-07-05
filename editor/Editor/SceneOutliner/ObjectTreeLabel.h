#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/TableRow.h>
#include "ITreeItem.h"
//#include "SceneOutlinerFwd.h"
#include "SceneOutlinerStandaloneTypes.h"//class SceneOutlinerCommonLabelData

namespace GuGu {
	class IMenu;
	class GameObject;
	struct ObjectTreeItem;
	class TransactionManager;
	namespace SceneOutlinerNameSpace
	{
		struct ObjectTreeLabel : public SceneOutlinerCommonLabelData, public CompoundWidget
		{
			struct BuilderArguments : public Arguments<ObjectTreeLabel>
			{
				BuilderArguments() {}

				~BuilderArguments() = default;

				//nothing
			};

			void init(const BuilderArguments& arguments, ObjectTreeItem& objectItem, ISceneOutliner& sceneOutliner,
				const TableRow<TreeItemPtr>& inRow);

			GuGuUtf8Str getDisplayText() const;

			Reply OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent);

			Reply OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

			Reply rightClick();

			Reply OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent) override;

			void OnDragEnter(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent) override;

			void OnDragLeave(const DragDropEvent& dragDropEvent) override;

			Reply OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent) override;

			Visibility getHoverVisibility() const;

			void deleteGameObjectsAndItsChildrens(std::shared_ptr<GameObject> inGameObject, TransactionManager& inTransactionManager);
		private:
			std::weak_ptr<ObjectTreeItem> m_treeItemPtr;
			std::weak_ptr<GameObject> m_objectPtr;

			//右键打开的菜单
			std::shared_ptr<IMenu> m_menu;

			bool m_bDragHover;
		};
	}
	
}