#pragma once

#include <vector>
#include <Core/GuGuUtf8Str.h>

namespace GuGu
{
	//asset registry
	struct ARFilter
	{
		std::vector<GuGuUtf8Str> m_classNames;

		bool isEmpty() const
		{
			return m_classNames.size() == 0;
		}
	};
}