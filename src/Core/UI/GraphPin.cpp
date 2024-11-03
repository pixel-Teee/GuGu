#include <pch.h>

#include "GraphPin.h"
#include "BoxPanel.h"
#include "ImageWidget.h"
#include "CoreStyle.h"

namespace GuGu {
	GraphPin::GraphPin()
	{
		m_bIsInput = true;
	}
	GraphPin::~GraphPin()
	{
	}
	void GraphPin::init(const BuilderArguments& arguments)
	{
		m_bIsInput = (arguments.mdirection == GraphPinDirection::Input);

		std::shared_ptr<Widget> pinWidget = WIDGET_NEW(ImageWidget).brush(CoreStyle::getStyleSet()->getBrush("graph.pin"));

		std::shared_ptr<HorizontalBox> pinContent;
		if (m_bIsInput)
		{
			pinContent = WIDGET_NEW(HorizontalBox)
						 + HorizontalBox::Slot()
						 .FixedWidth()
						 .setVerticalAlignment(VerticalAlignment::Center)
						 (
							pinWidget
						 );

				//todo:add label
				//+ HorizontalBox::Slot()
				//.FixedWidth()
				//.setVerticalAlignment(VerticalAlignment::Center)
				//(
				//
				//);
		}
		else
		{
			pinContent = WIDGET_NEW(HorizontalBox)
						+ HorizontalBox::Slot()
						.FixedWidth()
						.setVerticalAlignment(VerticalAlignment::Center)
						(
							pinWidget
						);
		}

		Border::init(Border::BuilderArguments()
			.Content(
				pinContent
			)
		);
	}
}