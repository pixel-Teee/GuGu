#include <pch.h>

#include "SceneOutliner.h"
#include <Core/UI/HeaderRow.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/TextBlockWidget.h>
#include "OutlinerTreeView.h"

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		void SceneOutliner::init(const BuilderArguments& arguments)
		{
			m_headerRowWidget = WIDGET_NEW(HeaderRow)
								.visibility(Visibility::Visible);//后续加个 optionals 在参数里面

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
		}

		Visibility SceneOutliner::getEmptyLabelVisibility() const
		{
			return (m_rootTreeItems.size() > 0) ? Visibility::Collapsed : Visibility::Visible;
		}

		std::shared_ptr<ITableRow> SceneOutliner::onGenerateRowForOutlinerTree(TreeItemPtr item, const std::shared_ptr<TableViewBase>& ownerTable)
		{
			return WIDGET_NEW(SceneOutlinerTreeRow, m_outlinerTreeView, std::static_pointer_cast<SceneOutliner>(shared_from_this())).item(item);
		}

	}
}