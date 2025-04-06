#include <pch.h>

#include "DetailItemNode.h"
#include "PropertyNode.h"
#include "DetailSingleItemRow.h"//DetailSingleItemRow widget

namespace GuGu {

	DetailItemNode::DetailItemNode(const DetailLayoutCustomization& inCustomization, std::shared_ptr<DetailCategoryImpl> inParentCategory)
		: m_customization(inCustomization)
		, m_parentCategory(inParentCategory)
	{
		setParentNode(inParentCategory);
	}

	DetailItemNode::~DetailItemNode()
	{

	}

	void DetailItemNode::getChildren(DetailNodeList& outChildren)
	{
		//todo:实现这个函数
	}

	std::shared_ptr<GuGu::ITableRow> DetailItemNode::generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable)
	{
		if (m_customization.hasPropertyNode() && m_customization.getPropertyNode()->asCategoryNode())
		{
			//return WIDGET_NEW()
			return nullptr;
		}
		else
		{
			return WIDGET_NEW(DetailSingleItemRow, &m_customization, false, shared_from_this(), ownerTable);
		}
	}

	void DetailItemNode::initialize()
	{

	}

}