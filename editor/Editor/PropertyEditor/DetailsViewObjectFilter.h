#pragma once

#include <vector>

namespace GuGu {
	class GameObject;
	struct DetailsViewObjectRoot
	{
		DetailsViewObjectRoot(){}

		DetailsViewObjectRoot(GameObject* inObject)
		{
			m_objects.push_back(inObject);
		}

		DetailsViewObjectRoot(const std::vector<GameObject*>& inObjects)
		{
			m_objects.reserve(inObjects.size());
			for (GameObject* object : inObjects)
			{
				m_objects.push_back(object);
			}
		}

		std::vector<GameObject*> m_objects;
	};

	class DetailsViewObjectFilter
	{
	public:
		virtual ~DetailsViewObjectFilter() {}

		virtual std::vector<DetailsViewObjectRoot> filterObjects(const std::vector<GameObject*>& sourceObjects) = 0;
	};
}