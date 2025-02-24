#include <pch.h>

#include "SceneOutlinerGutter.h"
#include <Core/UI/Spacer.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/NullWidget.h>

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		SceneOutlinerGutter::SceneOutlinerGutter(ISceneOutliner& outliner)
		{
			
		}

		const std::shared_ptr<GuGu::Widget> SceneOutlinerGutter::constructRowWidget(SceneOutlinerNameSpace::TreeItemPtr treeItem, const TableRow<SceneOutlinerNameSpace::TreeItemPtr>& row)
		{
			return WIDGET_NEW(HorizontalBox)
				   + HorizontalBox::Slot()
				   .FixedWidth()
				   .setVerticalAlignment(VerticalAlignment::Center)
				   (
					   NullWidget::getNullWidget() //todo:添加 visibility widget
				   );
		}

		HeaderRow::GColumn::BuilderArguments SceneOutlinerGutter::constructHeaderRowColumn()
		{
			return HeaderRow::Column(getColumnID())
				   .FixedWidth(4.0f)
				   .headerContent
				   (
						WIDGET_NEW(Spacer)//列头(Header)的内容
				   );
		}
	}
	
}