#include <pch.h>

#include "PropertyNode.h"

namespace GuGu {

	PropertyNode::PropertyNode()
	{

	}

	PropertyNode::~PropertyNode()
	{

	}

	void PropertyNode::initNode(const PropertyNodeInitParams& initParams)
	{
		destoryTree();

		m_parentNode = initParams.m_parentNode.get();
		m_parentNodeWeakPtr = initParams.m_parentNode;

		//property data
		m_property = initParams.m_property;

		initBeforeNodeFlags();

		rebuildChildren();
	}

	void PropertyNode::rebuildChildren()
	{
		destoryTree();

		if (m_childNodes.size() == 0)
		{
			initChildNodes();
		}
	}

	void PropertyNode::addChildNode(std::shared_ptr<PropertyNode> inNode)
	{
		m_childNodes.push_back(inNode);
	}

	ComplexPropertyNode* PropertyNode::findComplexParent()
	{
		PropertyNode* cur = this;
		ComplexPropertyNode* found = nullptr;
		while (true)
		{
			found = cur->asComplexNode();
			if (found)
			{
				break;
			}
			cur = cur->getParentNode();
			if (!cur)
			{
				break;
			}
		}
		return found;
	}

	ObjectPropertyNode* PropertyNode::findObjectItemParent()
	{
		ComplexPropertyNode* complexParent = findComplexParent();
		if (!complexParent)
		{
			return nullptr;
		}
		if (ObjectPropertyNode* objectNode = complexParent->asObjectNode())
		{
			return objectNode;
		}
		else if (PropertyNode* parentNodePtr = complexParent->getParentNode())
		{
			return parentNodePtr->findObjectItemParent();
		}
		return nullptr;
	}

	void PropertyNode::destoryTree()
	{
		m_childNodes.clear();
	}

}