#include <pch.h>

#include "Graph.h"

#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
	Graph::Graph()
	{
	}
	Graph::~Graph()
	{
	}
	void Graph::init(const BuilderArguments& arguments)
	{
	}
	uint32_t Graph::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
	{
		ArrangedWidgetArray arrangedChildren;
		AllocationChildActualSpace(allocatedGeometry, arrangedChildren);

		return layer;
	}
}
