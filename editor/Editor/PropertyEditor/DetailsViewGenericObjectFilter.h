#pragma once

#include "DetailsViewObjectFilter.h"

namespace GuGu {
	class DetailsViewDefaultObjectFilter : public DetailsViewObjectFilter
	{
	public:
		DetailsViewDefaultObjectFilter(bool bInAllowMultipleRoots);//允许多个根

		virtual std::vector<DetailsViewObjectRoot> filterObjects(const std::vector<GameObject*>& sourceObjects);

	private:
		bool m_bAllowMultipleRoots;
	};
}