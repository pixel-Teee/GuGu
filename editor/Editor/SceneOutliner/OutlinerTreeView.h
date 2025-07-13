#pragma once

#include <Core/UI/TreeView.h>
#include "SceneOutlinerFwd.h"

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		class OutlinerTreeView : public TreeView<TreeItemPtr> //ITreeItem
		{
		public:
			//struct BuilderArguments : public Arguments<OutlinerTreeView>
			//{
			//	BuilderArguments() {}
			//
			//	~BuilderArguments() = default;
			//};

			void init(const BuilderArguments& arguments, std::shared_ptr<SceneOutliner> owner);
		protected:

			std::weak_ptr<SceneOutliner> m_sceneOutlinerWeak;
		};

		class SceneOutlinerTreeRow : public MultiColumnTableRow<TreeItemPtr>
		{
		public:
			struct BuilderArguments : public Arguments<SceneOutlinerTreeRow>
			{
				BuilderArguments() {}
			
				~BuilderArguments() = default;

				ARGUMENT_VALUE(TreeItemPtr, item)

				ARGUMENT_ATTRIBUTE(std::shared_ptr<TableRowStyle>, Style)
			};

			void init(const BuilderArguments& arguments, const std::shared_ptr<OutlinerTreeView>& outlinerTreeView, std::shared_ptr<SceneOutliner> sceneOutliner);

			//这个函数是由 MultiColumnTableRow::init 调用，进行生成

			//SceneOutlinerTreeRow::init->SceneOutlinerTreeRow::generateWidgetForColumn->ISceneOutlinerColumn::constructRowWidget
			std::shared_ptr<Widget> generateWidgetForColumn(const GuGuUtf8Str& inColumnName) override;

		private:
			std::weak_ptr<SceneOutliner> m_sceneOutlinerWeak;

			std::weak_ptr<ITreeItem> m_item;

			static Reply handleOnDragDetected(const WidgetGeometry& inWidgetGeometry, const PointerEvent& inMouseEvent, std::weak_ptr<OutlinerTreeView> table);
		};
	}
}