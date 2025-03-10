#pragma once

#include "IDetailPropertyRow.h"
#include <memory>

namespace GuGu {
	class PropertyNode;
	class DetailCategoryImpl;
	class DetailPropertyRow : public IDetailPropertyRow, public std::enable_shared_from_this<DetailPropertyRow>
	{
	public:
		DetailPropertyRow(std::shared_ptr<PropertyNode> inPropertyNode, std::shared_ptr<DetailCategoryImpl> inParentCategory);
	};
}