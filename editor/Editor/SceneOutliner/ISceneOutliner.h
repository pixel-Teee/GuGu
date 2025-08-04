#pragma once

#include <Core/UI/CompoundWidget.h> //compound widget

namespace GuGu {
	class ISceneOutlinerMode;
	class ISceneOutliner : public CompoundWidget
	{
	public:
		virtual ~ISceneOutliner() {}

		virtual void refresh() = 0;

		virtual void fullRefresh() = 0;

		const std::shared_ptr<ISceneOutlinerMode> getMode() const { return m_mode; }

		virtual std::shared_ptr<Widget> getParentWindow() const = 0;
	protected:
		std::shared_ptr<ISceneOutlinerMode> m_mode;
	};
}