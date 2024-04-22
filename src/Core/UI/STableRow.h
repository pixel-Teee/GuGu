#pragma once

#include "Border.h"
#include "ITableRow.h"

namespace GuGu {
	template<typename ItemType>
	class TableRow : public ITableRow, public Border
	{

	};
}