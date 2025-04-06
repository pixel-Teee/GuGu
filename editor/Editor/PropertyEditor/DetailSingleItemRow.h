#pragma once

#include "DetailTableRowBase.h"
#include "DetailWidgetRow.h" //detail widget row，存放控件

namespace GuGu {
	class PropertyNode;
	struct DetailLayoutCustomization;
	class DetailSingleItemRow : public DetailTableRowBase
	{
	public:
		struct BuilderArguments : public Arguments<DetailSingleItemRow>
		{
			BuilderArguments()
			{
				//this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;	
		};

		void init(const BuilderArguments& inArgs, DetailLayoutCustomization* inCustomization, bool bHasMultipleColumns,
			std::shared_ptr<DetailTreeNode> inOwnerTreeNode, const std::shared_ptr<TableViewBase>& inOwnerTableView);

	private:
		DetailLayoutCustomization* m_customization;
		DetailWidgetRow m_widgetRow;
		std::shared_ptr<PropertyNode> m_swappablePropertyNode;
		std::shared_ptr<Button> m_expanderArrow;

	};
}