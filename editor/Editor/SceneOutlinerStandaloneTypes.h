#pragma once

namespace GuGu {
	class ISceneOutliner;
	struct SceneOutlinerCommonLabelData
	{
		std::weak_ptr<ISceneOutliner> m_weakSceneOutliner;
	};
}