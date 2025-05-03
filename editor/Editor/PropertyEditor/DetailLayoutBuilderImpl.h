#pragma once

#include "DetailLayoutBuilder.h"
#include "DetailTreeNode.h" //DetailNodeList
#include "PropertyRowGenerator.h"//CategoryMap

namespace GuGu {
	class IPropertyHandle;
	class PropertyNode;
	class DetailLayoutBuilderImpl : public IDetailLayoutBuilder
	{
	public:
		DetailLayoutBuilderImpl(std::shared_ptr<ComplexPropertyNode>& inRootNode);

		virtual ~DetailLayoutBuilderImpl();

		//构造 tree view 的 detail node 的数组
		DetailNodeList& getFilteredRootTreeNodes() { return m_filteredRootTreeNodes;}

		void generateDetailLayout();

		//构建可以显示的目录
		void buildCategories(const CategoryMap& categoryMap, 
							std::vector<std::shared_ptr<DetailCategoryImpl>>& outSimpleCategories,
							std::vector<std::shared_ptr<DetailCategoryImpl>>& outAdvancedCategories);

		std::shared_ptr<ComplexPropertyNode> getRootNode() const { return m_rootNode.lock(); }

		DetailCategoryImpl& defaultCategory(GuGuUtf8Str categoryName);

		std::shared_ptr<IPropertyHandle> getPropertyHandle(std::shared_ptr<PropertyNode> propertyNodePtr) const;
	private:
		DetailNodeList m_filteredRootTreeNodes;

		DetailNodeList m_allRootTreeNodes;

		CategoryMap m_customCategoryMap;

		std::weak_ptr<ComplexPropertyNode> m_rootNode;

		CategoryMap m_defaultCategoryMap;
	};
}