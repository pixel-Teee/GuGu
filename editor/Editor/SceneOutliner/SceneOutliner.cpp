#include <pch.h>

#include "SceneOutliner.h"
#include "SceneOutlinerGutter.h"
#include "SceneOutlinerItemLabelColumn.h"
#include "Editor/StyleSet/EditorStyleSet.h"
#include "ObjectDragDropOperation.h"
#include "ObjectTreeItem.h"
#include "OutlinerTreeView.h"
#include "ObjectBrowingMode.h"
#include <Core/UI/HeaderRow.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/Button.h>
#include <Core/UI/WidgetPath.h>
#include <Core/UI/IMenu.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Editor/Transaction/TransactionManager.h>
#include <Application/Application.h>

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		void SceneOutliner::init(const BuilderArguments& arguments, std::shared_ptr<Widget> inParentWindow)
		{
			m_parentWindow = inParentWindow;

			m_bFullRefresh = true;
			m_bSortDirty = true;
			m_bNeedsColumnRefresh = true;

			m_sortByName = BuiltInColumnTypes::Label();//根据标签列来排序
			m_sortMode = ColumnSortMode::Ascending;//升序排序默认

			m_headerRowWidget = WIDGET_NEW(HeaderRow)
								.Style(EditorStyleSet::getStyleSet()->getStyle<HeaderRowStyle>("SceneOutliner.header"))
								.visibility(Visibility::Visible);//后续加个 optionals 在参数里面

			//创建列头，以及如何描述构造每列的widget和标头的widget
			setupColumns(*m_headerRowWidget);

			std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

			//todo：加个搜索框

			verticalBox->addSlot()
			.StretchHeight(1.0f)
			.setPadding(Padding(10.0f, 10.0f, 10.0f, 10.0f))
			(
				WIDGET_NEW(Overlay)
				+ Overlay::Slot()
				.setHorizontalAlignment(HorizontalAlignment::Stretch)
				.setVerticalAlignment(VerticalAlignment::Stretch)
				(
					WIDGET_NEW(Border) //背景
					.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige5"))
					.Content
					(
						NullWidget::getNullWidget()
					)
				)
				+ Overlay::Slot() //没东西的时候，显示空
				.setHorizontalAlignment(HorizontalAlignment::Center)
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_NEW(TextBlockWidget)
					.visibility(Attribute<Visibility>::CreateSP(this, &SceneOutliner::getEmptyLabelVisibility))
					.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
					.text("Empty Label")
				)
				+ Overlay::Slot()
				.setHorizontalAlignment(HorizontalAlignment::Stretch)
				.setVerticalAlignment(VerticalAlignment::Stretch)
				(
					WIDGET_ASSIGN_NEW(OutlinerTreeView, m_outlinerTreeView, std::static_pointer_cast<SceneOutliner>(shared_from_this()))
					.visibility(Visibility::Visible)
					//树的 header
					.headerRow(m_headerRowWidget)

					.treeItemSource(&m_rootTreeItems)

					.onGenerateRow(this, &SceneOutliner::onGenerateRowForOutlinerTree)

					.onGetChildren(this, &SceneOutliner::onGetChildrenForOutlinerTree)

					.onMouseButtonDoubleClick(this, &SceneOutliner::onOutlinerTreeDoubleClick)

					.onSelectionChanged(this, &SceneOutliner::onOutlinerTreeSelectionChanged)

					.itemHeight(18)
				)
			);

			//填充数据集
			populate();

			//std::shared_ptr<Border> border = 
			//WIDGET_NEW(Border)
			//.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige5"))
			//.Content
			//(
			//	verticalBox
			//);
			m_mode = std::make_shared<ObjectBrowsingMode>(this);

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_childWidget = verticalBox;
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		Visibility SceneOutliner::getEmptyLabelVisibility() const
		{
			return (m_rootTreeItems.size() > 0) ? Visibility::Collapsed : Visibility::Visible;
		}

		Reply SceneOutliner::OnMouseButtonDown(const WidgetGeometry& geometry, const PointerEvent& inMouseEvent)
		{
			if (inMouseEvent.getEffectingButton() == Keys::RightMouseButton)
			{
				std::shared_ptr<Widget> menuContent;
				menuContent = WIDGET_NEW(Button)
					.Clicked(this, &SceneOutliner::rightClick)
					.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
						.text("add game object")
					);

				WidgetPath widgetPath = WidgetPath();
				m_menu = Application::getApplication()->pushMenu(shared_from_this(), widgetPath, menuContent, inMouseEvent.m_screenSpacePosition);
				
				return Reply::Handled();
			}
			return Reply::Unhandled();
		}

		Reply SceneOutliner::rightClick()
		{
			m_menu->dismiss();
			//add object
			std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
			std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>();
			gameObject->addComponent(transformComponent);
			std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
			if (currentLevel)
			{
				//undo/redo
				TransactionManager& transactionManager = TransactionManager::getTransactionManager();
				transactionManager.beginTransaction();
				transactionManager.modifyObject(currentLevel);
				currentLevel->addGameObject(gameObject);
				transactionManager.commit();
			}

			return Reply::Handled();
		}

		std::shared_ptr<Widget> SceneOutliner::getParentWindow() const
		{
			return m_parentWindow.lock();
		}

		Reply SceneOutliner::OnDrop(const WidgetGeometry& myGeometry, const DragDropEvent& dragDropEvent)
		{
			std::shared_ptr<ObjectDragDropOperation> operation = dragDropEvent.getOperationAs<ObjectDragDropOperation>();
			if (operation)
			{
				std::shared_ptr<meta::Object> object = operation->getObject();
				if (object->GetType() == typeof(GameObject))
				{
					std::shared_ptr<GameObject> gameObject = std::static_pointer_cast<GameObject>(object);
					if (gameObject)
					{
						TransactionManager& transactionManager = TransactionManager::getTransactionManager();
						transactionManager.beginTransaction();
						std::shared_ptr<GameObject> parentObject = gameObject->getParentGameObject().lock();
						//termination of relationship
						Array<std::shared_ptr<GameObject>> childrens = parentObject->getChildrens();
						auto it = std::find(childrens.begin(), childrens.end(), object);
						if (it != childrens.end())
						{
							transactionManager.modifyObject(parentObject);
							transactionManager.modifyObject(gameObject);
							childrens.erase(it);
							gameObject->setParentGameObject(std::shared_ptr<GameObject>());
						}
						transactionManager.commit();
						World::getWorld()->getCurrentLevel()->refreshLevel();
						return Reply::Handled();
					}
				}
			}
			return Reply::Unhandled();
		}

		void SceneOutliner::populate()
		{
			//发生了距离的变化
			bool bMadeAnySignificantChanges = false;
			//填充列表数据
			if (m_bFullRefresh)
			{
				m_outlinerTreeView->clearSelection();

				rePopulateEntireTree();

				m_bFullRefresh = false;
				bMadeAnySignificantChanges = true;
			}

			const int32_t end = std::min((int32_t)m_pendingOperations.size(), 500);
			for (int32_t index = 0; index < end; ++index)
			{
				auto& pendingOp = m_pendingOperations[index];
				switch (pendingOp.m_type)
				{
					case PendingTreeOperation::Added:
					{
						bMadeAnySignificantChanges = addItemToTree(pendingOp.m_item) || bMadeAnySignificantChanges;
						break;
					}
				}
			}

			m_pendingOperations.erase(m_pendingOperations.begin(), m_pendingOperations.begin() + end);

			if (bMadeAnySignificantChanges)
			{
				m_bSortDirty = true;
			}
		}

		void SceneOutliner::refresh()
		{
			m_bFullRefresh = true;
		}

		void SceneOutliner::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
		{
			populate();

			if (m_bSortDirty)
			{
				m_outlinerTreeView->requestTreeRefresh();

				m_bSortDirty = false;
			}	
		}

		void SceneOutliner::onLevelChanged()
		{
			refresh();
		}

		void SceneOutliner::rePopulateEntireTree()
		{
			emptyTreeItems();

			std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
			const Array<std::shared_ptr<GameObject>>& gameObjects = currentLevel->getGameObjects();

			for (const auto& gameObject : gameObjects)
			{
				constructItemFor<ObjectTreeItem>(gameObject);
			}
		}

		bool SceneOutliner::addItemToTree(TreeItemPtr inItem)
		{
			//unfiltered表示添加一个未过滤的item到树里面
			addUnfilteredItemToTree(inItem);

			return true;
		}

		void SceneOutliner::addUnfilteredItemToTree(TreeItemPtr item)
		{
			TreeItemPtr parent = ensureParentForItem(item);

			const TreeItemID itemId = item->getID();
			//m_rootTreeItems.push_back(item);

			if (m_treeItemMap.find(itemId) != m_treeItemMap.end())
			{
				GuGu_LOGE("tree item error");
			}

			m_treeItemMap.insert({itemId, item});
			if (parent)
			{
				parent->addChild(item);
			}
			else
			{
				m_rootTreeItems.push_back(item);
			}
		}

		TreeItemPtr SceneOutliner::ensureParentForItem(TreeItemPtr item)
		{
			TreeItemPtr parent = item->findParent(m_treeItemMap);
			if (parent)
			{
				return parent;
			}
			else
			{
				auto newParent = item->createParent();
				if (newParent)
				{
					addUnfilteredItemToTree(newParent);
					return newParent;
				}
			}
			return nullptr;
		}

		void SceneOutliner::emptyTreeItems()
		{
			m_rootTreeItems.clear();
			m_treeItemMap.clear();
		}

		void SceneOutliner::setupColumns(HeaderRow& headerRow)
		{
			headerRow.clearColumns();

			std::vector<GuGuUtf8Str> sortedIDs;
			sortedIDs.reserve(2);
			sortedIDs.push_back("0");//sort id
			sortedIDs.push_back("1");//sort id

			for (const GuGuUtf8Str& id : sortedIDs)
			{
				std::shared_ptr<ISceneOutlinerColumn> column;

				if (id == "0")
				{
					column = std::make_shared<SceneOutlinerGutter>(*std::static_pointer_cast<ISceneOutliner>(shared_from_this()));
				}
				if (id == "1")
				{
					column = std::make_shared<SceneOutlinerItemLabelColumn>(*std::static_pointer_cast<ISceneOutliner>(shared_from_this()));
				}
				if (column)
				{
					m_columns.insert({ column->getColumnID(), column });
				}

				auto columnArgs = column->constructHeaderRowColumn();//column 的构造参数

				//todo:添加 sort mode 和 on sort

				if (id == "1")
					columnArgs.DefaultLabel("ItemLabel");//标签

				headerRow.addColumn(columnArgs);
			}

			m_bNeedsColumnRefresh = false;
		}

		std::shared_ptr<ITableRow> SceneOutliner::onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable)
		{
			return WIDGET_NEW(SceneOutlinerTreeRow, m_outlinerTreeView, std::static_pointer_cast<SceneOutliner>(shared_from_this()))
				   .item(item)
				   .Style(EditorStyleSet::getStyleSet()->getStyle<TableRowStyle>(u8"tablerow.beige"));
		}

		void SceneOutliner::onGetChildrenForOutlinerTree(TreeItemPtr inParent, std::vector<TreeItemPtr>& outChildren)
		{
			for (auto& weakChild : inParent->getChildren())
			{
				auto child = weakChild.lock();
				outChildren.push_back(child);

				//todo:排序
			}
		}

		void SceneOutliner::onOutlinerTreeDoubleClick(TreeItemPtr treeItem)
		{
			//onOutlinerTreeDoubleClick
		}

		void SceneOutliner::onOutlinerTreeSelectionChanged(TreeItemPtr treeItem, SelectInfo::Type selectInfo)
		{
			if (selectInfo == SelectInfo::Direct) //通过代码设置
			{
				return;
			}

			//SceneOutlinerItemSelection selection(treeItem);
			m_mode->onItemSelectionChanged(treeItem, selectInfo, SceneOutlinerItemSelection(*m_outlinerTreeView));
		}

		SceneOutlinerItemSelection::SceneOutlinerItemSelection(const std::vector<TreeItemPtr>& inSelectedItems)
		{
			for (const auto& item : inSelectedItems)
			{
				m_selectedItems.push_back(item);
			}
		}

		SceneOutlinerItemSelection::SceneOutlinerItemSelection(OutlinerTreeView& tree)
		: SceneOutlinerItemSelection(tree.getSelectedItems())
		{
			 
		}

	}
}