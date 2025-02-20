#include <pch.h>

#include "OutlinerTreeView.h"

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

			auto args = SuperRowType::BuilderArguments();

			MultiColumnTableRow::init(args, outlinerTreeView);
		}

		std::shared_ptr<GuGu::Widget> SceneOutlinerTreeRow::generateWidgetForColumn(const GuGuUtf8Str& inColumnName)
		{
			return nullptr;
		}

	}
}