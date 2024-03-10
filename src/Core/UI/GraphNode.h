#pragma once

#include "NodePanel.h"

namespace GuGu {
	class VerticalBox;
	class GraphNode : public Node
	{
	public:
		GraphNode();

		virtual ~GraphNode();
	private:
		std::shared_ptr<VerticalBox> m_leftNodeBox;
		
		std::shared_ptr<VerticalBox> m_rightBox;
	};
}