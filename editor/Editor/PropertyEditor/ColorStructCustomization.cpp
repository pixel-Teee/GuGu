#include <pch.h>

#include "ColorStructCustomization.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include <Core/UI/ColorBlock.h>
#include <Core/UI/Events.h>
#include <Core/UI/Box.h>
#include <Core/UI/Border.h>

#include <Editor/UI/ColorPicker.h>

namespace GuGu {

	std::shared_ptr<ColorStructCustomization> ColorStructCustomization::create()
	{
		return std::make_shared<ColorStructCustomization>();
	}

	std::shared_ptr<Widget> ColorStructCustomization::createColorWidget(std::weak_ptr<IPropertyHandle>)
	{
		return WIDGET_NEW(BoxWidget)
		.padding(Padding(4.0f, 0, 4.0f, 0.0f))
		.VAlign(VerticalAlignment::Center)
		.Content
		(
			WIDGET_NEW(Border)
			.padding(1.0f)
			.verticalAlignment(VerticalAlignment::Center)
			.horizontalAlignment(HorizontalAlignment::Center)
			.Content
			(
				WIDGET_ASSIGN_NEW(ColorBlock, m_colorPickerParentWidget)
				.OnMouseButtonDown(this, &ColorStructCustomization::onMouseButtonDownColorBlock)
				.color(this, &ColorStructCustomization::onGetColorForColorBlock)
				.Size(math::float2(70.0f, 20.0f))
				.cornerRadius(math::float4(4.0f, 4.0f, 4.0f, 4.0f))
			)
		);
	}

	void ColorStructCustomization::createColorPicker(bool bUseAlpha)
	{
		Color initialColor;
		getColor(initialColor);
		ColorPickerArgs pickerArgs;
		{
			pickerArgs.m_bUseAlpha = bUseAlpha;
			pickerArgs.m_parentWidget = m_colorPickerParentWidget;
			pickerArgs.m_onColorCommitted = std::bind(&ColorStructCustomization::onSetColorFromColorPicker, this, std::placeholders::_1);
			pickerArgs.m_parentWidget = m_colorPickerParentWidget;
			pickerArgs.m_initialColorOverride = initialColor;
			pickerArgs.m_bOpenAsMenu = false;
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

	void ColorStructCustomization::onSetColorFromColorPicker(Color newColor)
	{
		GuGuUtf8Str colorStr = newColor.toStr();
		m_structPropertyHandle->setValueFromFormattedString(colorStr);
	}

	PropertyAccess::Result ColorStructCustomization::getColor(Color& outColor) const
	{
		outColor.a = 1.0f;

		GuGuUtf8Str strValue;
		PropertyAccess::Result result = m_structPropertyHandle->getValueAsFormattedString(strValue);

		if (result == PropertyAccess::Success)
		{
			outColor.initFromStr(strValue);
		}

		return result;
	}

	Color ColorStructCustomization::onGetColorForColorBlock() const
	{
		Color color;
		getColor(color);
		return color;
	}

}