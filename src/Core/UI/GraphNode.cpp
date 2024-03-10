#include <pch.h>

#include "GraphNode.h"
#include "BoxPanel.h"

namespace GuGu {
	GraphNode::GraphNode()
	{
	}
	GraphNode::~GraphNode()
	{
	}
	void GraphNode::updateGraphNode()
	{
		m_inputPins.clear();
		m_outputPins.clear();

		m_leftNodeBox = std::make_shared<VerticalBox>();
		m_rightBox = std::make_shared<VerticalBox>();

		//todo:add some pin
	}
}