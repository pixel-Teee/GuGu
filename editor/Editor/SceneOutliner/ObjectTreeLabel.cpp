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
#include <Core/Timer.h>
#include <Core/UI/TableRow.h>
#include <Core/UI/EditableText.h>

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		void ObjectTreeLabel::init(const BuilderArguments& arguments, ObjectTreeItem& objectItem, ISceneOutliner& sceneOutliner, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& inRow)
		{
			m_weakSceneOutliner = std::static_pointer_cast<ISceneOutliner>(sceneOutliner.shared_from_this());

			m_treeItemPtr = std::static_pointer_cast<ObjectTreeItem>(objectItem.shared_from_this());
			m_objectPtr = objectItem.m_gameObject;

			m_ownerRow = std::static_pointer_cast<TableRow<SceneOutlinerNameSpace::TreeItemPtr>>(std::const_pointer_cast<Widget>(inRow.shared_from_this()));

			m_bShowName = true;

			auto mainContent = WIDGET_NEW(HorizontalBox)
			//main actor label
			+ HorizontalBox::Slot()
			.FixedWidth()
			.setHorizontalAlignment(HorizontalAlignment::Left)
			.setVerticalAlignment(VerticalAlignment::Center)
			(
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.StretchHeight(1.0f)
				(
					WIDGET_NEW(BoxWidget)
					.HeightOverride(5.0f)
					.Content
					(
						WIDGET_NEW(Border)
						//.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getHoverVisibility))
						.BorderBackgroundColor(Attribute<math::float4>::CreateSP(this, &ObjectTreeLabel::onHoverTopBorder))
						.Content
						(
							NullWidget::getNullWidget()
						)
					)
				)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_NEW(Border)
					//.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getHoverVisibility))
					.BorderBackgroundColor(Attribute<math::float4>::CreateSP(this, &ObjectTreeLabel::onHoverCenter))
					.Content
					(
						WIDGET_NEW(VerticalBox)
						+ VerticalBox::Slot()
						.FixedHeight()
						(
							WIDGET_NEW(TextBlockWidget)
							.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getShowNameVisibility))
							.text(Attribute<GuGuUtf8Str>::CreateSP(this, &ObjectTreeLabel::getDisplayText))
							.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
						)
						+ VerticalBox::Slot()
						.FixedHeight()
						(
							WIDGET_NEW(EditableText)
							.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getEditNameVisibility))
							.text(Attribute<GuGuUtf8Str>::CreateSP(this, &ObjectTreeLabel::getDisplayText))
							.onTextCommitted(this, &ObjectTreeLabel::onRenameCommitted)
						)
					)
				)
				+ VerticalBox::Slot()
				.StretchHeight(1.0f)
				(
					WIDGET_NEW(BoxWidget)
					.HeightOverride(5.0f)
					.Content
					(
						WIDGET_NEW(Border)
						//.visibility(Attribute<Visibility>::CreateSP(this, &ObjectTreeLabel::getHoverVisibility))
						.BorderBackgroundColor(Attribute<math::float4>::CreateSP(this, &ObjectTreeLabel::onHoverBottomBorder))
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

			m_bDragHoverTopBorder = m_bDragHoverBottomBorder = m_bDragHoverCenter = false;
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
				//return Reply::Handled().detectDrag(shared_from_this(), Keys::LeftMouseButton);

				//rename
				Application::getApplication()->getTimer()->registerCallback(1.5f, [&](int32_t callbackId) {
					if (m_ownerRow.lock() && m_ownerRow.lock()->isSelected())
					{
						m_bShowName = false;
						//enter rename
						GuGu_LOGD("enter rename");
					}
					Application::getApplication()->getTimer()->removeCallback(callbackId);
				});
				return Reply::Unhandled();
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

		Reply ObjectTreeLabel::OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
		{
			if (inMouseEvent.getEffectingButton() == Keys::LeftMouseButton)
			{
				//trigger rename
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
				//World::getWorld()->getCurrentLevel()->deleteGameObject(m_objectPtr.lock());
				deleteGameObjectsAndItsChildrens(m_objectPtr.lock(), transactionManager);
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
						parentWindowLocked->onItemSelect(emptyGameObjects, true);
					}
				}
			}

			return Reply::Handled();
		}

		Reply ObjectTreeLabel::OnDragDetected(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent)
		{
			//note:注意，必须判断鼠标是否按下，因为OnDragDected在鼠标移动的时候触发，不然鼠标松开也会出现拖动的装饰性窗口
			if(m_objectPtr.lock() && mouseEvent.isMouseButtonDown(Keys::LeftMouseButton))
				return Reply::Handled().beginDragDrop(ObjectDragDropOperation::New(m_objectPtr.lock()));
			else
				return Reply::Unhandled();
		}

		void ObjectTreeLabel::OnDragEnter(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			//check hover zone
			math::float2 localPosition = myGeometry.absoluteToLocal(dragDropEvent.m_screenSpacePosition);
			if (localPosition.y <= 5.f)
			{
				m_bDragHoverTopBorder = true;
				m_bDragHoverCenter = false;
				m_bDragHoverBottomBorder = false;
			}
			else if (localPosition.y < myGeometry.getLocalSize().y - 5.f)
			{
				m_bDragHoverTopBorder = false;
				m_bDragHoverCenter = true;
				m_bDragHoverBottomBorder = false;
			}
			else
			{
				m_bDragHoverTopBorder = false;
				m_bDragHoverCenter = false;
				m_bDragHoverBottomBorder = true;
			}
		}

		void ObjectTreeLabel::OnDragLeave(const DragDropEvent& dragDropEvent)
		{
			m_bDragHoverTopBorder = m_bDragHoverCenter = m_bDragHoverBottomBorder = false;
		}

		Reply ObjectTreeLabel::OnDragOver(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			//check hover zone
			math::float2 localPosition = myGeometry.absoluteToLocal(dragDropEvent.m_screenSpacePosition);
			if (localPosition.y <= 5.f)
			{
				m_bDragHoverTopBorder = true;
				m_bDragHoverCenter = false;
				m_bDragHoverBottomBorder = false;
			}
			else if (localPosition.y < myGeometry.getLocalSize().y - 5.f)
			{
				m_bDragHoverTopBorder = false;
				m_bDragHoverCenter = true;
				m_bDragHoverBottomBorder = false;
			}
			else
			{
				m_bDragHoverTopBorder = false;
				m_bDragHoverCenter = false;
				m_bDragHoverBottomBorder = true;
			}

			return Reply::Unhandled();
		}

		Reply ObjectTreeLabel::OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			std::shared_ptr<ObjectDragDropOperation> operation = dragDropEvent.getOperationAs<ObjectDragDropOperation>();
			if (operation)
			{
				std::shared_ptr<meta::Object> draggedObject = operation->getObject();
				if (draggedObject->GetType() == typeof(GameObject))
				{
					std::shared_ptr<meta::Object> parentObject = m_objectPtr.lock();
					if (parentObject)
					{
						TransactionManager& transactionManager = TransactionManager::getTransactionManager();
						transactionManager.beginTransaction();
						//transactionManager.modifyObject(World::getWorld()->getCurrentLevel());
						std::shared_ptr<GameObject> childObject = std::static_pointer_cast<GameObject>(draggedObject);
						std::shared_ptr<GameObject> originParentObject = childObject->getParentGameObject().lock();
						if (originParentObject)
						{
							//termination of relationship
							Array<std::shared_ptr<GameObject>>& childrens = originParentObject->getChildrens();
							auto it = std::find(childrens.begin(), childrens.end(), childObject);
							if (it != childrens.end())
							{
								transactionManager.modifyObject(originParentObject);
								childrens.erase(it);
							}

							//detach
							if (!World::getWorld()->m_onObjectDetched.empty())
							{
								for (uint32_t i = 0; i < World::getWorld()->m_onObjectDetched.size(); ++i)
								{
									World::getWorld()->m_onObjectDetched[i](childObject, originParentObject);
								}
							}
						}

						//attach 3 situation
						if (m_bDragHoverCenter)
						{
							std::shared_ptr<GameObject> parent2Object = std::static_pointer_cast<GameObject>(parentObject);
							transactionManager.modifyObject(parent2Object);
							transactionManager.modifyObject(childObject);
							parent2Object->addChildren(childObject);

							//attach
							if (!World::getWorld()->m_onObjectAttached.empty())
							{
								for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
								{
									World::getWorld()->m_onObjectAttached[i](childObject, parent2Object);
								}
							}
						}
						else if (m_bDragHoverTopBorder)
						{
							std::shared_ptr<GameObject> droppedGameObject = std::static_pointer_cast<GameObject>(draggedObject);
							//get index
							std::shared_ptr<GameObject> lockedGameObject = m_objectPtr.lock();
							if (lockedGameObject->getParentGameObject().lock() != nullptr)
							{
								std::shared_ptr<GameObject> parentGameObject = lockedGameObject->getParentGameObject().lock();
								int32_t index = parentGameObject->findIndex(lockedGameObject);
								if (index != -1)
								{
									transactionManager.modifyObject(parentGameObject);
									transactionManager.modifyObject(droppedGameObject);
									parentGameObject->insertChildren(droppedGameObject, index);
								}
								else
								{
									GuGu_LOGE("object tree label find index error");
								}

								//attach
								if (!World::getWorld()->m_onObjectAttached.empty())
								{
									for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
									{
										World::getWorld()->m_onObjectAttached[i](droppedGameObject, parentGameObject);
									}
								}
							}
							else //level
							{
								std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
								int32_t index = currentLevel->findIndex(lockedGameObject);

								if (index != -1)
								{
									transactionManager.modifyObject(currentLevel);
									transactionManager.modifyObject(droppedGameObject);
									currentLevel->insertChildren(droppedGameObject, index);
								}
								else
								{
									GuGu_LOGE("object tree label find index error");
								}

								std::shared_ptr<GameObject> makedNullGameObject = std::make_shared<GameObject>();

								//attach
								if (!World::getWorld()->m_onObjectAttached.empty())
								{
									for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
									{
										World::getWorld()->m_onObjectAttached[i](droppedGameObject, makedNullGameObject);
									}
								}
							}
						}
						else if(m_bDragHoverBottomBorder)
						{
							std::shared_ptr<GameObject> droppedGameObject = std::static_pointer_cast<GameObject>(draggedObject);
							//get index
							std::shared_ptr<GameObject> lockedGameObject = m_objectPtr.lock();
							if (lockedGameObject->getParentGameObject().lock() != nullptr)
							{
								std::shared_ptr<GameObject> parentGameObject = lockedGameObject->getParentGameObject().lock();
								int32_t index = parentGameObject->findIndex(lockedGameObject);
								if (index != -1)
								{
									transactionManager.modifyObject(parentGameObject);
									transactionManager.modifyObject(droppedGameObject);
									parentGameObject->insertChildren(droppedGameObject, index + 1);
								}
								else
								{
									GuGu_LOGE("object tree label find index error");
								}

								//attach
								if (!World::getWorld()->m_onObjectAttached.empty())
								{
									for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
									{
										World::getWorld()->m_onObjectAttached[i](droppedGameObject, parentGameObject);
									}
								}
							}
							else //level
							{
								std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
								int32_t index = currentLevel->findIndex(lockedGameObject);

								if (index != -1)
								{
									transactionManager.modifyObject(currentLevel);
									transactionManager.modifyObject(droppedGameObject);
									currentLevel->insertChildren(droppedGameObject, index + 1);
								}
								else
								{
									GuGu_LOGE("object tree label find index error");
								}

								std::shared_ptr<GameObject> makedNullGameObject = std::make_shared<GameObject>();

								//attach
								if (!World::getWorld()->m_onObjectAttached.empty())
								{
									for (uint32_t i = 0; i < World::getWorld()->m_onObjectAttached.size(); ++i)
									{
										World::getWorld()->m_onObjectAttached[i](droppedGameObject, makedNullGameObject);
									}
								}
							}
						}

						transactionManager.commit();

						m_bDragHoverTopBorder = m_bDragHoverBottomBorder = m_bDragHoverCenter = false;
						//World::getWorld()->getCurrentLevel()->refreshLevel();
						return Reply::Handled();
					}
				}
			}
			return Reply::Unhandled();
		}

		void ObjectTreeLabel::deleteGameObjectsAndItsChildrens(std::shared_ptr<GameObject> inGameObject, TransactionManager& inTransactionManager)
		{
			inGameObject->setParentGameObject(std::shared_ptr<GameObject>());
			auto& childrens = inGameObject->getChildrens();
			for (size_t i = 0; i < childrens.size(); ++i)
			{
				//childrens[i]->setParentGameObject(std::shared_ptr<GameObject>());
				deleteGameObjectsAndItsChildrens(childrens[i], inTransactionManager);
			}
			childrens.clear();
			World::getWorld()->getCurrentLevel()->deleteGameObject(inGameObject);
		}

		math::float4 ObjectTreeLabel::onHoverTopBorder() const
		{
			if (m_bDragHoverTopBorder)
				return EditorStyleSet::getStyleSet()->getColor("beige9");
			return math::float4(1.0f, 1.0f, 1.0f, 0.0f);//transparent
		}

		math::float4 ObjectTreeLabel::onHoverBottomBorder() const
		{
			if (m_bDragHoverBottomBorder)
				return EditorStyleSet::getStyleSet()->getColor("beige9");
			return math::float4(1.0f, 1.0f, 1.0f, 0.0f);//transparent
		}

		math::float4 ObjectTreeLabel::onHoverCenter() const
		{
			if (m_bDragHoverCenter)
				return EditorStyleSet::getStyleSet()->getColor("beige8");
			return math::float4(1.0f, 1.0f, 1.0f, 0.0f);//transparent
		}

		Visibility ObjectTreeLabel::getEditNameVisibility() const
		{
			if (m_bShowName)
				return Visibility::Collapsed;
			return Visibility::Visible;
		}

		Visibility ObjectTreeLabel::getShowNameVisibility() const
		{
			if (m_bShowName)
				return Visibility::Visible;
			return Visibility::Collapsed;
		}

		void ObjectTreeLabel::onRenameCommitted(const GuGuUtf8Str& text, TextCommit::Type commitType)
		{
			if (commitType == TextCommit::OnEnter)
			{
				m_objectPtr.lock()->setName(text);

				if (const TreeItemPtr treeItem = m_treeItemPtr.lock())
				{
					treeItem->setDisplayString(text);
				}

				m_bShowName = true;//quit edit name
			}
		}

	}

}