#pragma once

#include "DetailTableRowBase.h"

namespace GuGu {
	class DetailCategoryTableRow : public DetailTableRowBase
	{
	public:
		struct BuilderArguments : public Arguments<DetailCategoryTableRow>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(GuGuUtf8Str, displayName)

			ARGUMENT_VALUE(std::shared_ptr<Widget>, headerContent)
		};

	void init(const BuilderArguments& inArgs, std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView);
	private:

	};
}