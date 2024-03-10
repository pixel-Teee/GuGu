#pragma once

#include "Border.h"
#include "GraphNode.h"

namespace GuGu {
	class GraphNode;

	enum GraphPinDirection
	{
		Input,
		Output,
		Max
	};

	class GraphPin : public Border
	{
	public:
		GraphPin();

		virtual ~GraphPin();

		struct BuilderArguments : public Arguments<GraphPin>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_VALUE(GraphPinDirection, direction)
		};

		void init(const BuilderArguments& arguments);
	protected:
		std::weak_ptr<GraphNode> m_ownerNodePtr;

		std::shared_ptr<Widget> m_pinImage;

		bool m_bIsInput;
	};
}