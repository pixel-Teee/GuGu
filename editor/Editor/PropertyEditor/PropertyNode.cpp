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


	}

	void PropertyNode::rebuildChildren()
	{
		destoryTree();

		if (m_childNodes.size() == 0)
		{
			initChildNodes();
		}
	}

	void PropertyNode::destoryTree()
	{
		m_childNodes.clear();
	}

}