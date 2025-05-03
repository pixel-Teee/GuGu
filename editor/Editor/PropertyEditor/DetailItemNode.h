#pragma once

#include "DetailTreeNode.h"
#include "DetailCategoryBuilderImpl.h"//DetailLayoutCustomization
#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	class DetailItemNode : public DetailTreeNode, public std::enable_shared_from_this<DetailItemNode>
	{
	public:
		DetailItemNode(const DetailLayoutCustomization& inCustomization, std::shared_ptr<DetailCategoryImpl> inParentCategory);

		virtual ~DetailItemNode();

		virtual void getChildren(DetailNodeList& outChildren) override;

		virtual std::shared_ptr<ITableRow> generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable) override;

		virtual GuGuUtf8Str getNodeName() const override { return m_customization.getName(); }

		void initialize();
	private:
		void initPropertyEditor();

		DetailLayoutCustomization m_customization;

		std::weak_ptr<DetailCategoryImpl> m_parentCategory;
	};
}