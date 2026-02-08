#include <pch.h>

#include "PropertyMenuGameObjectPicker.h"
#include <Core/UI/Box.h>
#include <Editor/SceneOutliner/SceneOutliner.h>
#include <Editor/SceneOutliner/SceneOutlinerTypes.h>
#include <Editor/SceneOutliner/ObjectPickMode.h> //pick mode

namespace GuGu {

	PropertyMenuGameObjectPicker::PropertyMenuGameObjectPicker()
	{

	}

	PropertyMenuGameObjectPicker::~PropertyMenuGameObjectPicker()
	{

	}

	void PropertyMenuGameObjectPicker::init(const BuilderArguments& arguments)
	{
		OnGameObjectSelected onGameObjectSelected = arguments.monGameObjectSelected;
		std::shared_ptr<Widget> menuContent;

		OnSceneOutlinerItemPicker onItemPicked = [onGameObjectSelected](std::shared_ptr<SceneOutlinerNameSpace::ITreeItem> inItem) {
			std::shared_ptr<ObjectTreeItem> treeItemPtr = std::static_pointer_cast<ObjectTreeItem>(inItem);
			if (treeItemPtr)
			{
				if (onGameObjectSelected)
				{
					onGameObjectSelected(treeItemPtr->m_gameObject.lock());
				}
			}
		};

		SceneOutlinerInitializationOptions options;
		options.m_modeFactory = [&onItemPicked](SceneOutlinerNameSpace::SceneOutliner* sceneOutliner) {
			std::shared_ptr<ObjectPickMode> pickMode = std::make_shared<ObjectPickMode>(sceneOutliner, onItemPicked);
			return pickMode;
		};

		menuContent = WIDGET_NEW(BoxWidget)
		.WidthOverride(300.0f)
		.HeightOverride(300.0f)
		.Content
		(
			WIDGET_NEW(SceneOutlinerNameSpace::SceneOutliner, options, nullptr) //reuse
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = menuContent;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

}