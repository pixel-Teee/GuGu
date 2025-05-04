#pragma once

#include "MathStructCustomization.h"

namespace GuGu {
	class WidgetGeometry;
	struct PointerEvent;
	class Reply;
	class ColorStructCustomization : public MathStructCustomization
	{
	public:
		static std::shared_ptr<ColorStructCustomization> create();

	protected:
		std::shared_ptr<Widget> createColorWidget(std::weak_ptr<IPropertyHandle>);

		void createColorPicker(bool bUseAlpha);

		Reply onMouseButtonDownColorBlock(const WidgetGeometry& myGeometry, const PointerEvent& mouseEvent);

		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) override;
		virtual void makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow) override;

	protected:
		std::shared_ptr<IPropertyHandle> m_structPropertyHandle;

		std::shared_ptr<Widget> m_colorPickerParentWidget;
	};
}