#include <pch.h>

#include "ObjectTreeLabel.h"
#include "ISceneOutliner.h"
#include "ObjectTreeItem.h"
#include <Editor/StyleSet/EditorStyleSet.h>
#include <Core/UI/BoxPanel.h> 

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
				WIDGET_NEW(TextBlockWidget)
				.text(Attribute<GuGuUtf8Str>::CreateSP(this, &ObjectTreeLabel::getDisplayText))
				.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
			);

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget = mainContent;
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		GuGuUtf8Str ObjectTreeLabel::getDisplayText() const
		{
			if (const TreeItemPtr treeItem = m_treeItemPtr.lock())
			{
				return GuGuUtf8Str(treeItem->getDisplayString());
			}
			return GuGuUtf8Str();
		}

	}

}