#pragma once

#include <Core/UI/CompoundWidget.h>

namespace GuGu {
	class GameObject;//game object
	class IDetailsView : public CompoundWidget
	{
	public:
		virtual void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) = 0;
	};
}