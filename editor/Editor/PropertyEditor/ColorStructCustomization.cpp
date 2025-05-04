#include <pch.h>

#include "ColorStructCustomization.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include <Core/UI/ColorBlock.h>
#include <Core/UI/Events.h>
#include <Core/UI/ColorPicker.h>

namespace GuGu {

	std::shared_ptr<ColorStructCustomization> ColorStructCustomization::create()
	{
		return std::make_shared<ColorStructCustomization>();
	}

	std::shared_ptr<Widget> ColorStructCustomization::createColorWidget(std::weak_ptr<IPropertyHandle>)
	{
		return WIDGET_ASSIGN_NEW(ColorBlock, m_colorPickerParentWidget)
			   .OnMouseButtonDown(this, &ColorStructCustomization::onMouseButtonDownColorBlock);
	}

	void ColorStructCustomization::createColorPicker(bool bUseAlpha)
	{
		ColorPickerArgs pickerArgs;
		{
			pickerArgs.m_bUseAlpha = !bUseAlpha;
			pickerArgs.m_parentWidget = m_colorPickerParentWidget;
		}

		openColorPicker(pickerArgs);
	}

	Reply ColorStructCustomization::onMouseButtonDownColorBlock(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent)
	{
		if(mouseEvent.getEffectingButton() != Keys::LeftMouseButton)
			return Reply::Unhandled();

		createColorPicker(true);

		return Reply::Handled();
	}

	void ColorStructCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		m_structPropertyHandle = propertyHandle;

		MathStructCustomization::customizeHeader(m_structPropertyHandle, headerRow);
	}

	void ColorStructCustomization::makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		std::shared_ptr<Widget> colorWidget;

		std::weak_ptr<IPropertyHandle> structWeakHandlePtr = m_structPropertyHandle;

		colorWidget = createColorWidget(structWeakHandlePtr);

		headerRow.nameContent()
		(
			m_structPropertyHandle->createPropertyNameWidget("")
		)
		.valueContent()
		(
			colorWidget
		);
	}

}