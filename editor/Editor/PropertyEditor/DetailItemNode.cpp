#include <pch.h>

#include "DetailItemNode.h"
#include "PropertyNode.h"
#include "DetailPropertyRow.h"
#include "DetailSingleItemRow.h"//DetailSingleItemRow widget
#include "DetailCategoryTableRow.h"

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
		for (int32_t childIndex = 0; childIndex < m_children.size(); ++childIndex)
		{
			std::shared_ptr<DetailTreeNode>& child = m_children[childIndex];

			if (child->shouldShowOnlyChildren())
			{
				child->getChildren(outChildren);
			}
			else
			{
				outChildren.push_back(child);
			}
		}
	}

	std::shared_ptr<GuGu::ITableRow> DetailItemNode::generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable)
	{
		if (m_customization.hasPropertyNode() && m_customization.getPropertyNode()->asCategoryNode())
		{
			//return WIDGET_NEW()
			return WIDGET_NEW(DetailCategoryTableRow, shared_from_this(), ownerTable, nullptr)
				.displayName(m_customization.getPropertyNode()->getDisplayName());
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

		generateChildren();
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

	void DetailItemNode::generateChildren()
	{
		DetailNodeList oldChildren = m_children;
		m_children.clear();

		std::shared_ptr<DetailCategoryImpl> parentCategoryLocked = m_parentCategory.lock();
		if (parentCategoryLocked == nullptr)
		{
			return;
		}

		if (m_customization.hasPropertyNode())
		{
			m_customization.m_propertyRow->onGenerateChildren(m_children);
		}
	}

	bool DetailItemNode::shouldShowOnlyChildren() const
	{
		return m_customization.hasPropertyNode() && m_customization.m_propertyRow->showOnlyChildren();
	}

	void DetailItemNode::initPropertyEditor()
	{
		m_customization.m_propertyRow->onItemNodeInitialized(m_parentCategory.lock());
	}

}