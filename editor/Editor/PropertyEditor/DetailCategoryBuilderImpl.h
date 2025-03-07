#pragma once

#include "DetailCategoryBuilder.h"//IDetailCategoryBuilder
#include "DetailTreeNode.h"//DetailTreeNode

namespace GuGu {
	class DetailCategoryImpl : public IDetailCategoryBuilder, public DetailTreeNode
	{
	public:
		DetailCategoryImpl();

		virtual ~DetailCategoryImpl();


	};
}