#pragma once

#include "DetailCategoryBuilder.h"//IDetailCategoryBuilder
#include "DetailTreeNode.h"//DetailTreeNode

namespace GuGu {
	class DetailPropertyRow;
	struct DetailLayoutCustomization
	{
		DetailLayoutCustomization();
		//属性细节的属性节点
		std::shared_ptr<DetailPropertyRow> m_propertyRow;

	};
	class DetailLayout
	{
	public:
		DetailLayout() { }

		const std::vector<DetailLayoutCustomization>& getSimpleLayouts() const { return m_simpleLayouts; }
	private:
		std::vector<DetailLayoutCustomization> m_simpleLayouts;
	};
	class DetailLayoutMap
	{
	public:

		DetailLayout& operator[](int32_t index) { return m_layouts[index]; }
		const DetailLayout& operator[](int32_t index) const { return m_layouts[index]; }
		int32_t size() const { return m_layouts.size(); }
	private:
		std::vector<DetailLayout> m_layouts;

	};
	class DetailCategoryImpl : public IDetailCategoryBuilder, public DetailTreeNode
	{
	public:
		DetailCategoryImpl();

		virtual ~DetailCategoryImpl();

		//构建这个目录的儿子
		void generateLayout();

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
	};
}