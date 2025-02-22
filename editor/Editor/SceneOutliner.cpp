#include <pch.h>

#include "SceneOutliner.h"
#include <Core/UI/HeaderRow.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/TextBlockWidget.h>
#include "OutlinerTreeView.h"
#include "SceneOutlinerGutter.h"

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		void SceneOutliner::init(const BuilderArguments& arguments)
		{
			m_bNeedsColumnRefresh = true;

			m_headerRowWidget = WIDGET_NEW(HeaderRow)
								.visibility(Visibility::Visible);//后续加个 optionals 在参数里面

			setupColumns(*m_headerRowWidget);

			std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

			verticalBox->addSlot()
			.StretchHeight(1.0f)
			(
				WIDGET_NEW(Overlay)
				+ Overlay::Slot()
				.setHorizontalAlignment(HorizontalAlignment::Center)
				(
					WIDGET_NEW(TextBlockWidget)
					.visibility(Attribute<Visibility>::CreateSP(this, &SceneOutliner::getEmptyLabelVisibility))
				)
				+ Overlay::Slot()
				(
					WIDGET_ASSIGN_NEW(OutlinerTreeView, m_outlinerTreeView, std::static_pointer_cast<SceneOutliner>(shared_from_this()))
					//树的 header
					.headerRow(m_headerRowWidget)

					.treeItemSource(&m_rootTreeItems)

					.onGenerateRow(this, &SceneOutliner::onGenerateRowForOutlinerTree)
				)
			);

			//todo: populate

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_childWidget = verticalBox;
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		Visibility SceneOutliner::getEmptyLabelVisibility() const
		{
			return (m_rootTreeItems.size() > 0) ? Visibility::Collapsed : Visibility::Visible;
		}

		void SceneOutliner::setupColumns(HeaderRow& headerRow)
		{
			headerRow.clearColumns();

			std::vector<GuGuUtf8Str> sortedIDs;
			sortedIDs.reserve(1);
			sortedIDs.push_back("0");//sort id

			for (const GuGuUtf8Str& id : sortedIDs)
			{
				std::shared_ptr<ISceneOutlinerColumn> column;

				if (id == "0")
				{
					column = std::make_shared<SceneOutlinerGutter>(*std::static_pointer_cast<ISceneOutliner>(shared_from_this()));
				}
				if (column)
				{
					m_columns.insert({ column->getColumnID(), column });
				}

				auto columnArgs = column->constructHeaderRowColumn();//column 的构造参数

				//todo:添加 sort mode 和 on sort

				headerRow.addColumn(columnArgs);
			}

			m_bNeedsColumnRefresh = false;
		}

		std::shared_ptr<ITableRow> SceneOutliner::onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable)
		{
			return WIDGET_NEW(SceneOutlinerTreeRow, m_outlinerTreeView, std::static_pointer_cast<SceneOutliner>(shared_from_this())).item(item);
		}

	}
}