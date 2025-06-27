#include <pch.h>

#include "ObjectTreeLabel.h"
#include "ISceneOutliner.h"
#include "ObjectTreeItem.h"
#include "ObjectDragDropOperation.h"
#include <Editor/StyleSet/EditorStyleSet.h>
#include <Editor/EditorMainWindow.h>
#include <Core/UI/BoxPanel.h> 
#include <Core/UI/Button.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/WidgetPath.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/World.h>
#include <Application/Application.h>
#include <Editor/Transaction/TransactionManager.h>

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		void ObjectTreeLabel::init(const BuilderArguments& arguments, ObjectTreeItem& objectItem, ISceneOutliner& sceneOutliner, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& inRow)
		{
			m_weakSceneOutliner = std::static_pointer_cast<ISceneOutliner>(sceneOutliner.shared_from_this());

			m_treeItemPtr = std::static_pointer_cast<ObjectTreeItem>(objectItem.shared_from_this());
			m_objectPtr = objectItem.m_gameObject;

			auto mainContent = WIDGET_NEW(HorizontalBox)
			//main actor label
			+ HorizontalBox::Slot()
			.FixedWidth()
			.setHorizontalAlignment(HorizontalAlignment::Left)
			.setVerticalAlignment(VerticalAlignment::Center)
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_NEW(TextBlockWidget)
					.text(Attribute<GuGuUtf8Str>::CreateSP(this, &ObjectTreeLabel::getDisplayText))
					.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
				)
				+ VerticalBox::Slot()
				.StretchHeight(1.0f)
				(
					WIDGET_NEW(BoxWidget)
					.HeightOverride(10.0f)
					.Content
					(
						WIDGET_NEW(Border)
						.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getHoverVisibility))
						.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
				)
			);

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget = mainContent;
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());

			m_bDragHover = false;
		}

		GuGuUtf8Str ObjectTreeLabel::getDisplayText() const
		{
			if (const TreeItemPtr treeItem = m_treeItemPtr.lock())
			{
				return GuGuUtf8Str(treeItem->getDisplayString());
			}
			return GuGuUtf8Str();
		}

		Reply ObjectTreeLabel::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
		{
			if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
			{
				//触发拖动
				//detect drag
				return Reply::Handled().detectDrag(shared_from_this(), Keys::LeftMouseButton);
			}
			else if (inMouseEvent.getEffectingButton() == Keys::RightMouseButton)
			{
				std::shared_ptr<Widget> menuContent;
				menuContent = WIDGET_NEW(Button)
					.Clicked(this, &ObjectTreeLabel::rightClick)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
						.text("delete game object")
					);

				WidgetPath widgetPath = WidgetPath();
				m_menu = Application::getApplication()->pushMenu(shared_from_this(), widgetPath, menuContent, inMouseEvent.m_screenSpacePosition);

				return Reply::Handled();
			}
			return Reply::Unhandled();
		}

		Reply ObjectTreeLabel::rightClick()
		{
			m_menu->dismiss();
			//delete object
			
			std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
			if (currentLevel)
			{
				//undo/redo
				TransactionManager& transactionManager = TransactionManager::getTransactionManager();
				transactionManager.beginTransaction();
				transactionManager.modifyObject(currentLevel);
				World::getWorld()->getCurrentLevel()->deleteGameObject(m_objectPtr.lock());
				transactionManager.commit();
			}	
	
			if (m_weakSceneOutliner.lock())
			{
				m_weakSceneOutliner.lock()->refresh();
				//m_weakSceneOutliner.lock()->
				std::shared_ptr<Widget> parentWindow = m_weakSceneOutliner.lock()->getParentWindow();
				if (parentWindow)
				{
					std::shared_ptr<EditorMainWindow> parentWindowLocked = std::static_pointer_cast<EditorMainWindow>(parentWindow);
					if (parentWindowLocked)
					{
						std::vector<GameObject*> emptyGameObjects;
						parentWindowLocked->refreshDetailsView(emptyGameObjects, true);
					}
				}
			}

			return Reply::Handled();
		}

		Reply ObjectTreeLabel::OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent)
		{
			if(m_objectPtr.lock())
				return Reply::Handled().beginDragDrop(ObjectDragDropOperation::New(m_objectPtr.lock()));
			else
				return Reply::Unhandled();
		}

		void ObjectTreeLabel::OnDragEnter(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			m_bDragHover = true;
		}

		void ObjectTreeLabel::OnDragLeave(const DragDropEvent& dragDropEvent)
		{
			m_bDragHover = false;
		}

		Reply ObjectTreeLabel::OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			std::shared_ptr<ObjectDragDropOperation> operation = dragDropEvent.getOperationAs<ObjectDragDropOperation>();
			if (operation)
			{
				std::shared_ptr<meta::Object> object = operation->getObject();
				if (object->GetType() == typeof(GameObject))
				{
					std::shared_ptr<meta::Object> parentObject = m_objectPtr.lock();
					if (parentObject)
					{
						TransactionManager& transactionManager = TransactionManager::getTransactionManager();
						transactionManager.beginTransaction();
						//transactionManager.modifyObject(World::getWorld()->getCurrentLevel());
						std::shared_ptr<GameObject> childObject = std::static_pointer_cast<GameObject>(object);
						std::shared_ptr<GameObject> originParentObject = childObject->getParentGameObject().lock();
						if (originParentObject)
						{
							//termination of relationship
							Array<std::shared_ptr<GameObject>> childrens = originParentObject->getChildrens();
							auto it = std::find(childrens.begin(), childrens.end(), childObject);
							if (it != childrens.end())
							{
								transactionManager.modifyObject(originParentObject);
								childrens.erase(it);
							}
						}

						std::shared_ptr<GameObject> parent2Object = std::static_pointer_cast<GameObject>(parentObject);
						transactionManager.modifyObject(parent2Object);
						transactionManager.modifyObject(childObject);
						parent2Object->addChildren(childObject);
						transactionManager.commit();
						World::getWorld()->getCurrentLevel()->refreshLevel();
						return Reply::Handled();
					}
				}
			}
			return Reply::Unhandled();
		}

		Visibility ObjectTreeLabel::getHoverVisibility() const
		{
			if(m_bDragHover)
				return Visibility::Visible;
			return Visibility::Collapsed;
		}

	}

}