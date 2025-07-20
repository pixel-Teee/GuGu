#include <pch.h>

#include "OutlinerTreeView.h"
#include "SceneOutliner.h"
#include "ISceneOutlinerColumn.h" 
#include "ObjectDragDropOperation.h"

namespace GuGu {

	namespace SceneOutlinerNameSpace
	{
		void OutlinerTreeView::init(const BuilderArguments& arguments, std::shared_ptr<SceneOutliner> owner)
		{
			m_sceneOutlinerWeak = owner;
			TreeView::init(arguments);
		}

		void SceneOutlinerTreeRow::init(const BuilderArguments& arguments, const std::shared_ptr<OutlinerTreeView>& outlinerTreeView, std::shared_ptr<SceneOutliner> sceneOutliner)
		{
			m_item = arguments.mitem;
			m_sceneOutlinerWeak = sceneOutliner;
			//m_style = arguments.mStyle.Get();

			auto args = SuperRowType::BuilderArguments();
			args.mStyle = arguments.mStyle.Get();

			//std::function<Reply(const WidgetGeometry&, const PointerEvent&)> testFunc = 
			//std::bind(&SceneOutlinerTreeRow::handleOnDragDetected, 
			//std::placeholders::_1, 
			//std::placeholders::_2, 
			//std::weak_ptr<OutlinerTreeView>(outlinerTreeView));

			args.onDragDetectedStatic(&SceneOutlinerTreeRow::handleOnDragDetected, std::weak_ptr<OutlinerTreeView>(outlinerTreeView));

			MultiColumnTableRow::init(args, outlinerTreeView);
		}

		std::shared_ptr<GuGu::Widget> SceneOutlinerTreeRow::generateWidgetForColumn(const GuGuUtf8Str& inColumnName)
		{
			auto itemPtr = m_item.lock();
			if (!itemPtr)
			{
				return NullWidget::getNullWidget();
			}

			auto outliner = m_sceneOutlinerWeak.lock();

			std::shared_ptr<Widget> newItemWidget = NullWidget::getNullWidget();
			
			auto columns = outliner->getColumns();
			auto it = columns.find(inColumnName);
			std::shared_ptr<ISceneOutlinerColumn> column = nullptr;
			if (it != columns.end())
			{
				column = it->second;
			}
			if (column)
			{
				newItemWidget = column->constructRowWidget(itemPtr, *this);
			}

			if (inColumnName == "ItemLabel")
			{
				//第一列，有 expansion arrow
				return WIDGET_NEW(HorizontalBox)
					   // HorizontalBox::Slot()
					   //FixedWidth()
					   //setPadding(Padding(6, 0, 0, 0))
					   //
					   //	WIDGET_NEW(ExpanderArrow, std::static_pointer_cast<SceneOutlinerTreeRow>(shared_from_this()))
					   //	.indentAmount(12)
					   //
					   + HorizontalBox::Slot()
					   .StretchWidth(1.0f)
					   (
							newItemWidget
					   );
			}
			else
			{
				//其他列，就是普通的内容
				return newItemWidget;
			}
		}

		Reply SceneOutlinerTreeRow::handleOnDragDetected(const WidgetGeometry& inWidgetGeometry, const PointerEvent& inMouseEvent, std::weak_ptr<OutlinerTreeView> table)
		{
			std::shared_ptr<OutlinerTreeView> lockedTable = table.lock();
			//note:注意，必须判断鼠标是否按下，因为OnDragDected在鼠标移动的时候触发，不然鼠标松开也会出现拖动的装饰性窗口
			if (inMouseEvent.isMouseButtonDown(Keys::LeftMouseButton))
			{
				const auto& items = lockedTable->getSelectedItems();
				if (items.size() > 0)
				{
					//get first item
					SceneOutlinerNameSpace::TreeItemPtr treeItem = items[0];
					std::shared_ptr<ObjectTreeItem> objectTreeItem = std::static_pointer_cast<ObjectTreeItem>(treeItem);
					if (objectTreeItem)
					{
						return Reply::Handled().beginDragDrop(ObjectDragDropOperation::New(objectTreeItem->m_gameObject.lock()));
					}
				}
				return Reply::Unhandled();
			}
			else
				return Reply::Unhandled();
		}

	}
}