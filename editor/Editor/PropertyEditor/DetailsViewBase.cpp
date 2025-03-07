#include <pch.h>

#include "DetailsViewBase.h"
#include "DetailLayoutBuilderImpl.h" //DetailLayoutBuilderImpl

namespace GuGu {

	DetailsViewBase::DetailsViewBase()
	{

	}

	DetailsViewBase::~DetailsViewBase()
	{

	}

	void DetailsViewBase::onGetChildrenForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, std::vector<std::shared_ptr<DetailTreeNode>>& outChildren)
	{
		return inTreeNode->getChildren(outChildren);
	}

	std::shared_ptr<ITableRow> DetailsViewBase::onGenerateRowForDetailTree(std::shared_ptr<DetailTreeNode> inTreeNode, const std::shared_ptr<TableViewBase>& ownerTable)
	{
		return inTreeNode->generateWidgetForTableView(ownerTable);
	}

	void DetailsViewBase::updateFilteredDetails()
	{
		m_rootTreeNodes.clear();

		DetailNodeList initialRootNodeList;
		RootPropertyNodeList& rootPropertyNodes = getRootNodes();//ComplexPropertyNode的数组

		for (int32_t rootNodeIndex = 0; rootNodeIndex < rootPropertyNodes.size(); ++rootNodeIndex)
		{
			std::shared_ptr<ComplexPropertyNode>& rootPropertyNode = rootPropertyNodes[rootNodeIndex];
			if (rootPropertyNode)
			{
				std::shared_ptr<DetailLayoutBuilderImpl>& detailLayout = m_detailLayouts[rootNodeIndex].m_detailLayout;
				if (detailLayout)
				{
					//DetailTreeNode的数组，DetailTreeNode 可用于构造 tree view
					DetailNodeList& layoutRoots = detailLayout->getFilteredRootTreeNodes();
					if (layoutRoots.size() > 0)
					{
						//append
						initialRootNodeList.insert(initialRootNodeList.end(), layoutRoots.begin(), layoutRoots.end());
					}
				}
			}
		}

		for (std::shared_ptr<DetailTreeNode> rootNode : initialRootNodeList)
		{
			m_rootTreeNodes.push_back(rootNode);
		}

		refreshTree();
	}

	void DetailsViewBase::refreshTree()
	{
		m_detailTree->requestTreeRefresh();
	}

	void DetailsViewBase::updateSinglePropertyMap(std::shared_ptr<ComplexPropertyNode> inRootPropertyNode, DetailLayoutData& layoutData, bool bIsExternal)
	{
		std::shared_ptr<DetailLayoutBuilderImpl> detailLayout = std::make_shared<DetailLayoutBuilderImpl>();
		layoutData.m_detailLayout = detailLayout;//builder impl

		std::shared_ptr<ComplexPropertyNode> rootPropertyNode = inRootPropertyNode;
		assert(rootPropertyNode != nullptr);

		layoutData.m_detailLayout->generateDetailLayout();//生成 tree node
	}

	void DetailsViewBase::updatePropertyMaps()
	{
		m_rootTreeNodes.clear();//detail tree node vector

		//属性节点表
		RootPropertyNodeList& rootPropertyNodes = getRootNodes();//complex property node vector

		m_detailLayouts.resize(rootPropertyNodes.size());

		for (int32_t rootNodeIndex = 0; rootNodeIndex < rootPropertyNodes.size(); ++rootNodeIndex)
		{
			DetailLayoutData& layoutData = m_detailLayouts[rootNodeIndex];
			updateSinglePropertyMap(rootPropertyNodes[rootNodeIndex], layoutData, false);//生成 tree node(用于显示的)
		}
	}

}