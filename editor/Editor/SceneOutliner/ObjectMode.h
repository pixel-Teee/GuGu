#pragma once

#include "ISceneOutlinerMode.h"

namespace GuGu {
	struct ObjectModeParams
	{
		ObjectModeParams() {}

		ObjectModeParams(SceneOutlinerNameSpace::SceneOutliner* inSceneOutliner, bool bInHideComponents = true)
			: m_sceneoutliner(inSceneOutliner),
			  m_bHideComponents(bInHideComponents)
		{}


		SceneOutlinerNameSpace::SceneOutliner* m_sceneoutliner = nullptr;
		bool m_bHideComponents = true;
	};

	class ObjectMode : public ISceneOutlinerMode
	{
	public:
		struct ItemSortOrder
		{
			enum Type
			{
				Level = 10,
				Folder = 20,
				Object = 30
			};
		};

		ObjectMode(const ObjectModeParams& params);
		virtual ~ObjectMode();

		//这个模式是否可以是可交互的？
		virtual bool isInteractive() const { return false; }

		bool m_bHideComponents;
	protected:
		SceneOutlinerNameSpace::SceneOutliner* m_sceneOutliner;
	};
}