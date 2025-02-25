#include <pch.h>

#include "SceneOutliner.h"
#include <Core/UI/HeaderRow.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/TextBlockWidget.h>
#include "OutlinerTreeView.h"
#include "SceneOutlinerGutter.h"
#include "SceneOutlinerItemLabelColumn.h"
#include "Editor/StyleSet/EditorStyleSet.h"
#include "ObjectTreeItem.h"
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		void SceneOutliner::init(const BuilderArguments& arguments)
		{
			m_bFullRefresh = true;
			m_bSortDirty = true;
			m_bNeedsColumnRefresh = true;

			m_headerRowWidget = WIDGET_NEW(HeaderRow)
								.Style(EditorStyleSet::getStyleSet()->getStyle<HeaderRowStyle>("SceneOutliner.header"))
								.visibility(Visibility::Visible);//后续加个 optionals 在参数里面

			setupColumns(*m_headerRowWidget);

			std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

			verticalBox->addSlot()
			.StretchHeight(1.0f)
			.setPadding(Padding(10.0f, 10.0f, 10.0f, 10.0f))
			(
				WIDGET_NEW(Overlay)
				+ Overlay::Slot()
				.setHorizontalAlignment(HorizontalAlignment::Center)
				.setVerticalAlignment(VerticalAlignment::Center)
				(
					WIDGET_NEW(TextBlockWidget)
					.visibility(Attribute<Visibility>::CreateSP(this, &SceneOutliner::getEmptyLabelVisibility))
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

					.itemHeight(18)
				)
			);

			//todo: populate
			populate();

			World::getWorld()->m_onLevelChanged = std::bind(&SceneOutliner::onLevelChanged, this);

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_childWidget = verticalBox;
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		Visibility SceneOutliner::getEmptyLabelVisibility() const
		{
			return (m_rootTreeItems.size() > 0) ? Visibility::Collapsed : Visibility::Visible;
		}

		void SceneOutliner::populate()
		{
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
			addUnfilteredItemToTree(inItem);

			return true;
		}

		void SceneOutliner::addUnfilteredItemToTree(TreeItemPtr item)
		{
			//todo:find parent

			m_rootTreeItems.push_back(item);
		}

		void SceneOutliner::emptyTreeItems()
		{
			m_rootTreeItems.clear();
		}

		void SceneOutliner::setupColumns(HeaderRow& headerRow)
		{
			headerRow.clearColumns();

			std::vector<GuGuUtf8Str> sortedIDs;
			sortedIDs.reserve(2);
			sortedIDs.push_back("0");//sort id
			sortedIDs.push_back("1");

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

		}

	}
}