#include <pch.h>

#include "DetailsViewGenericObjectFilter.h"

namespace GuGu {

	DetailsViewDefaultObjectFilter::DetailsViewDefaultObjectFilter(bool bInAllowMultipleRoots)
		: m_bAllowMultipleRoots(bInAllowMultipleRoots)
	{

	}

	std::vector<GuGu::DetailsViewObjectRoot> DetailsViewDefaultObjectFilter::filterObjects(const std::vector<GameObject*>& sourceObjects)
	{
		std::vector<DetailsViewObjectRoot> roots;
		if (m_bAllowMultipleRoots)
		{
			roots.reserve(sourceObjects.size());
			for (GameObject* object : sourceObjects)
			{
				roots.emplace_back(object);
			}
		}
		else
		{
			roots.emplace_back(sourceObjects);
		}
		return roots;
	}

}