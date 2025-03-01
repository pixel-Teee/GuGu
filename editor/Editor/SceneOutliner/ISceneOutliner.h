#pragma once

#include <Core/UI/CompoundWidget.h> //compound widget

namespace GuGu {
	class ISceneOutlinerMode;
	class ISceneOutliner : public CompoundWidget
	{
	public:
		virtual ~ISceneOutliner() {}

		virtual void refresh() = 0;

		const std::shared_ptr<ISceneOutlinerMode> getMode() const { return m_mode; }
	protected:
		std::shared_ptr<ISceneOutlinerMode> m_mode;
	};
}