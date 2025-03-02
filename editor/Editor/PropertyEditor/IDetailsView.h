#pragma once

#include <Core/UI/CompoundWidget.h>

namespace GuGu {
	class GameObject;//game object
	class IDetailsView : public CompoundWidget
	{
	public:
		//bForceRefresh 如果为真，不会检查 new objects 是否被设置
		//bOverrideLock 如果为真，会设置 objects ，即使 details view 被锁定
		virtual void setObjects(const std::vector<GameObject*>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) = 0;
		virtual void setObjects(const std::vector<std::weak_ptr<GameObject>>& inObjects, bool bForceRefresh = false, bool bOverrideLock = false) = 0;
	};
}