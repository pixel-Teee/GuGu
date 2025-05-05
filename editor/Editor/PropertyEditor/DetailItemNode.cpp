#include <pch.h>

#include "DetailItemNode.h"
#include "PropertyNode.h"
#include "DetailPropertyRow.h"
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
		if (m_customization.hasPropertyNode())
		{
			initPropertyEditor();
		}
	}

	IDetailsViewPrivate* DetailItemNode::getDetailsView() const
	{
		std::shared_ptr<DetailCategoryImpl> category = getParentCategory();
		return category ? category->getDetailsView() : nullptr;
	}

	IDetailsView* DetailItemNode::getNodeDetailsView() const
	{
		std::shared_ptr<DetailCategoryImpl> category = getParentCategory();
		return category ? category->getNodeDetailsView() : nullptr;
	}

	std::shared_ptr<DetailCategoryImpl> DetailItemNode::getParentCategory() const
	{
		return m_parentCategory.lock();
	}

	void DetailItemNode::initPropertyEditor()
	{
		m_customization.m_propertyRow->onItemNodeInitialized(m_parentCategory.lock());
	}

}