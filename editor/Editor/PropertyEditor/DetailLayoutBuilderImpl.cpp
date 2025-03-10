#include <pch.h>

#include "DetailLayoutBuilderImpl.h"
#include "DetailCategoryBuilderImpl.h" //目录树节点

namespace GuGu {

	DetailLayoutBuilderImpl::DetailLayoutBuilderImpl()
	{

	}

	DetailLayoutBuilderImpl::~DetailLayoutBuilderImpl()
	{

	}

	void DetailLayoutBuilderImpl::generateDetailLayout()
	{
		//生成 tree node
		m_allRootTreeNodes.clear();

		std::vector<std::shared_ptr<DetailCategoryImpl>> simpleCategories;//tree nodes
		std::vector<std::shared_ptr<DetailCategoryImpl>> advancedOnlyCategories;//tree nodes
		//todo:排序 category

		while (m_customCategoryMap.size() > 0)
		{
			CategoryMap customCategoryMapCopy = m_customCategoryMap;
			m_customCategoryMap.clear();

			//todo:build categories
			buildCategories(customCategoryMapCopy, simpleCategories, advancedOnlyCategories);
		}

		DetailNodeList categoryNodes;
		
		{
			for (uint32_t i = 0; i < simpleCategories.size(); ++i)
			{
				std::shared_ptr<DetailTreeNode> detailTreeNode = simpleCategories[i];
				categoryNodes.push_back(detailTreeNode);
			}
		}

		std::shared_ptr<ComplexPropertyNode> rootNodeLocked = m_rootNode.lock();//property node

		m_allRootTreeNodes = categoryNodes;
	}

	void DetailLayoutBuilderImpl::buildCategories(const CategoryMap& categoryMap, 
		std::vector<std::shared_ptr<DetailCategoryImpl>>& outSimpleCategories, 
		std::vector<std::shared_ptr<DetailCategoryImpl>>& outAdvancedCategories)
	{
		for (const auto& item : categoryMap)
		{
			std::shared_ptr<DetailCategoryImpl> detailCategory = item.second;

			std::shared_ptr<ComplexPropertyNode> rootPropertyNode = getRootNode();
			const bool bCategoryHidden = false;
			if (!bCategoryHidden)
			{
				detailCategory->generateLayout();

				outSimpleCategories.push_back(detailCategory);
			}
		}
	}



}