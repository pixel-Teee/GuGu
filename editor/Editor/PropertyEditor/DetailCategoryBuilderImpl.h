#pragma once

#include "DetailCategoryBuilder.h"//IDetailCategoryBuilder
#include "DetailTreeNode.h"//DetailTreeNode
#include "DetailWidgetRow.h"

namespace GuGu {
	class DetailPropertyRow;
	class PropertyNode;
	struct DetailLayoutCustomization
	{
		DetailLayoutCustomization();
		//属性细节的属性节点
		std::shared_ptr<DetailPropertyRow> m_propertyRow;

		GuGuUtf8Str getName() const;

		bool hasPropertyNode() const { return getPropertyNode() != nullptr; }

		bool isValidCustomization() const { return hasPropertyNode(); }

		std::shared_ptr<PropertyNode> getPropertyNode() const;

		DetailWidgetRow getWidgetRow() const;
	};
	class DetailLayout
	{
	public:
		DetailLayout(GuGuUtf8Str instanceName) : m_instanceName(instanceName) { }

		const GuGuUtf8Str& getInstanceName() const { return m_instanceName; }

		const std::vector<DetailLayoutCustomization>& getSimpleLayouts() const { return m_simpleLayouts; }

		void addLayout(const DetailLayoutCustomization& layout);
	private:
		std::vector<DetailLayoutCustomization> m_simpleLayouts;

		GuGuUtf8Str m_instanceName;
	};
	class DetailLayoutMap
	{
	public:

		DetailLayout& operator[](int32_t index) { return m_layouts[index]; }
		const DetailLayout& operator[](int32_t index) const { return m_layouts[index]; }
		int32_t size() const { return m_layouts.size(); }

		DetailLayout& findOrAdd(GuGuUtf8Str instanceName)
		{
			for (int32_t layoutIndex = 0; layoutIndex < m_layouts.size(); ++layoutIndex)
			{
				DetailLayout& layout = m_layouts[layoutIndex];
				if (layout.getInstanceName() == instanceName)
				{
					return layout;
				}
			}

			m_layouts.push_back(DetailLayout(instanceName));
			int32_t index = m_layouts.size() - 1;//the end
			return m_layouts[index];
		}
	private:
		std::vector<DetailLayout> m_layouts;

	};
	
	class DetailLayoutBuilderImpl;
	class DetailCategoryImpl : public IDetailCategoryBuilder, public DetailTreeNode, public std::enable_shared_from_this<DetailCategoryImpl>
	{
	public:
		DetailCategoryImpl(const GuGuUtf8Str& categoryName, std::shared_ptr<DetailLayoutBuilderImpl> inDetailLayoutBuilderImpl);

		virtual ~DetailCategoryImpl();

		//构建这个目录的儿子
		void generateLayout();

		virtual GuGuUtf8Str getNodeName() const override { return getCategoryName(); }

		GuGuUtf8Str getCategoryName() const { return m_categoryName; }

		void addPropertyNode(std::shared_ptr<PropertyNode> propertyNode, GuGuUtf8Str instanceName);

		void addDefaultLayout(const DetailLayoutCustomization& defaultLayoutInfo, GuGuUtf8Str instanceName);

		DetailLayout& getLayoutForInstance(GuGuUtf8Str instanceName);

		virtual void getChildren(DetailNodeList& outChildren) override;

		void getGeneratedChildren(DetailNodeList& outChildren, bool bIgnoreVisibility, bool bIgnoreAdvancedDropdown);

		virtual std::shared_ptr<ITableRow> generateWidgetForTableView(const std::shared_ptr<TableViewBase>& ownerTable) override;

		virtual const GuGuUtf8Str& getDisplayName() const override { return m_displayName; }

		void setDisplayName(GuGuUtf8Str inCategoryName);

		DetailLayoutBuilderImpl& getParentLayoutImpl() const { return *m_detailLayoutBuilder.lock(); }

		virtual IDetailsViewPrivate* getDetailsView() const override;

		virtual IDetailsView* getNodeDetailsView() const override;

		//std::shared_ptr<Widget> onGetContextMenuContent() const;
		virtual bool shouldShowOnlyChildren() const override;
	private:
		//生成儿子的所有布局
		void generateChildrenForLayouts();

		//在一个简单的布局从一个自定义列表生成节点
		void generateNodesFromCustomizations(const std::vector<DetailLayoutCustomization>& inCustomizationList, DetailNodeList& outNodeList);

		//显示在这个目录的布局
		DetailLayoutMap m_layoutMap;

		//所有简单儿子节点
		std::vector<std::shared_ptr<DetailTreeNode>> m_simpleChildNodes;

		//一个属性节点，显示在 header row ，在目录名右边
		std::shared_ptr<DetailTreeNode> m_inlinePropertyNode;

		GuGuUtf8Str m_categoryName;

		std::shared_ptr<Widget> m_headerContentWidget;

		GuGuUtf8Str m_displayName;

		std::weak_ptr<DetailLayoutBuilderImpl> m_detailLayoutBuilder;

		bool m_bShowOnlyChildren;
	};
}