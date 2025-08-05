#pragma once

#include "ObjectMode.h"

namespace GuGu {
	class GameObject;
	class ObjectModeInteractive : public ObjectMode
	{
	public:
		ObjectModeInteractive(const ObjectModeParams& params);
		virtual ~ObjectModeInteractive();

		virtual bool isInteractive() const override { return true; }

		void onLevelChanged();

		void onObjectAdded(std::shared_ptr<GameObject>& inObjects);

		void onObjectRemoved(std::shared_ptr<GameObject>& inObject);

		void onObjectAttached(std::shared_ptr<GameObject>& inObject, std::shared_ptr<GameObject>& inParent);

		void onObjectDetached(std::shared_ptr<GameObject>& inObject, std::shared_ptr<GameObject>& inParent);
	};
}