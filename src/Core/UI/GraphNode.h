#pragma once

#include "NodePanel.h"

namespace GuGu {
	class GraphPin;
	class VerticalBox;
	class GraphNode : public Node
	{
	public:
		GraphNode();

		virtual ~GraphNode();

		void updateGraphNode();
	private:
		std::vector<std::shared_ptr<GraphPin>> m_inputPins;

		std::vector<std::shared_ptr<GraphPin>> m_outputPins;

		std::shared_ptr<VerticalBox> m_leftNodeBox;
		
		std::shared_ptr<VerticalBox> m_rightBox;
	};
}