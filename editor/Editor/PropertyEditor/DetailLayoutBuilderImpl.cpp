#include <pch.h>

#include "DetailLayoutBuilderImpl.h"
#include "DetailCategoryBuilderImpl.h" //目录树节点
#include "PropertyHandleImp.h"

namespace GuGu {

	DetailLayoutBuilderImpl::DetailLayoutBuilderImpl(std::shared_ptr<ComplexPropertyNode>& inRootNode, const std::shared_ptr<IDetailsViewPrivate>& inDetailsView)
		: m_rootNode(inRootNode)
		, m_detailsView(inDetailsView.get())
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

		while (m_defaultCategoryMap.size() > 0)
		{
			CategoryMap defaultCategoryMapCopy = m_defaultCategoryMap;
			m_defaultCategoryMap.clear();

			buildCategories(defaultCategoryMapCopy, simpleCategories, advancedOnlyCategories);
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
		m_filteredRootTreeNodes = m_allRootTreeNodes;
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
				detailCategory->generateLayout();//这个关键，会产生目录下的 detail item node

				outSimpleCategories.push_back(detailCategory);
			}
		}
	}

	DetailCategoryImpl& DetailLayoutBuilderImpl::defaultCategory(GuGuUtf8Str categoryName)
	{
		for (const std::shared_ptr<DetailTreeNode>& rootTreeNode : m_allRootTreeNodes)
		{
			if (rootTreeNode->getNodeName() == categoryName)
			{
				return (DetailCategoryImpl&)(*rootTreeNode);
			}
		}
		//find or add
		std::shared_ptr<DetailCategoryImpl> categoryImpl;
		if (m_defaultCategoryMap.find(categoryName) != m_defaultCategoryMap.end())
		{
			categoryImpl = m_defaultCategoryMap.find(categoryName)->second;
		}
		else
		{
			m_defaultCategoryMap.insert({ categoryName, std::make_shared<DetailCategoryImpl>(categoryName, std::static_pointer_cast<DetailLayoutBuilderImpl>(shared_from_this())) });
			categoryImpl = m_defaultCategoryMap.find(categoryName)->second;

			categoryImpl->setDisplayName(categoryName);
		}

		return *categoryImpl;
	}

	std::shared_ptr<IPropertyHandle> DetailLayoutBuilderImpl::getPropertyHandle(std::shared_ptr<PropertyNode> propertyNodePtr) const
	{
		std::shared_ptr<IPropertyHandle> propertyHandle;
		if (propertyNodePtr)
		{
			std::shared_ptr<PropertyNode> propertyNode = propertyNodePtr;
			if (PropertyHandleFloat::supports(propertyNode)) //todo:这个后面要封装成函数
			{
				propertyHandle = std::make_shared<PropertyHandleFloat>(propertyNode);
			}
			else if (PropertyHandleVector3::supports(propertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleVector3>(propertyNode);
			}
			else if (PropertyHandleRotator::supports(propertyNode))
			{
				propertyHandle = std::make_shared<PropertyHandleRotator>(propertyNode);
			}
			else
			{
				propertyHandle = std::make_shared<PropertyHandleBase>(propertyNode);
			}
		}
		else
		{
			//invalid handle
		}
		return propertyHandle;
	}

}