#include <pch.h>

#include "MathStructCustomization.h"
#include "DetailWidgetRow.h"
#include <Core/UI/TextBlockWidget.h>

namespace GuGu {

	void MathStructCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		makeHeaderRow(propertyHandle, headerRow);
	}

	void MathStructCustomization::makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		headerRow.nameContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text(u8"kksk")
			.textColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
		)
		.valueContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text(u8"kksk2")
			.textColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f))
		);
	}

}