#include <pch.h>

#include "DetailCategoryBuilderImpl.h"
#include "DetailItemNode.h"

namespace GuGu {

	DetailCategoryImpl::DetailCategoryImpl()
	{

	}

	DetailCategoryImpl::~DetailCategoryImpl()
	{

	}

	void DetailCategoryImpl::generateLayout()
	{
		m_simpleChildNodes.clear();
		m_inlinePropertyNode.reset();
		generateChildrenForLayouts();
	}

	void DetailCategoryImpl::generateChildrenForLayouts()
	{
		for (int32_t layoutIndex = 0; layoutIndex < m_layoutMap.size(); ++layoutIndex)
		{
			const DetailLayout& layout = m_layoutMap[layoutIndex];
			generateNodesFromCustomizations(layout.getSimpleLayouts(), m_simpleChildNodes);
		}
	}

	void DetailCategoryImpl::generateNodesFromCustomizations(const std::vector<DetailLayoutCustomization>& inCustomizationList, 
		 DetailNodeList& outNodeList)
	{
		std::vector<DetailLayoutCustomization> customizationList = inCustomizationList;
		for (const DetailLayoutCustomization& customization : customizationList)
		{
			std::shared_ptr<DetailItemNode> newNode = std::make_shared<DetailItemNode>();

			outNodeList.push_back(newNode);
		}
	}

	DetailLayoutCustomization::DetailLayoutCustomization()
	{

	}

}