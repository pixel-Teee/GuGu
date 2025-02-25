#pragma once

#include <Core/UI/CompoundWidget.h> //compound widget

namespace GuGu {
	class ISceneOutliner : public CompoundWidget
	{
	public:
		virtual ~ISceneOutliner() {}

		virtual void refresh() = 0;
	};
}