#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/TableRow.h>
#include "ITreeItem.h"
//#include "SceneOutlinerFwd.h"
#include "SceneOutlinerStandaloneTypes.h"//class SceneOutlinerCommonLabelData

namespace GuGu {
	class EditableText;
	class IMenu;
	class GameObject;
	struct ObjectTreeItem;
	class TransactionManager;
	struct CallBackInfo;
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

			Reply rightClickDeleteGameObject();

			Reply rightClickAddChildGameObject();

			Reply OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent) override;

			void OnDragEnter(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent) override;

			void OnDragLeave(const DragDropEvent& dragDropEvent) override;

			Reply OnDragOver(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent) override;

			Reply OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent) override;

			void deleteGameObjectsAndItsChildrens(std::shared_ptr<GameObject> inGameObject, TransactionManager& inTransactionManager);

			math::float4 onHoverTopBorder() const;

			math::float4 onHoverBottomBorder() const;

			math::float4 onHoverCenter() const;

			Visibility getEditNameVisibility() const;

			Visibility getShowNameVisibility() const;

			void onRenameCommitted(const GuGuUtf8Str&, TextCommit::Type);

			virtual bool supportsKeyboardFocus() const;
		private:
			std::weak_ptr<ObjectTreeItem> m_treeItemPtr;
			std::weak_ptr<GameObject> m_objectPtr;

			std::shared_ptr<EditableText> m_editableText;

			std::weak_ptr<TableRow<TreeItemPtr>> m_ownerRow;

			//右键打开的菜单
			std::shared_ptr<IMenu> m_menu;

			bool m_bShowName;
			bool m_bDragHoverTopBorder;
			bool m_bDragHoverCenter;
			bool m_bDragHoverBottomBorder;

			std::weak_ptr<CallBackInfo> m_activeTimer;
		};
	}
	
}