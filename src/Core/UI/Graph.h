#pragma once

#include "NodePanel.h"

namespace GuGu {
	class Graph : public NodePanel
	{
	public:
		Graph();

		virtual ~Graph();

		struct BuilderArguments : public Arguments<Graph>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;		
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;
	private:

	};
}