#include <pch.h>

#include "AnchorDataCustomization.h"
#include "PropertyHandleImp.h"
#include "DetailPropertyRow.h"
#include "CustomChildBuilder.h"//IDetailChildrenBuilder
#include <Core/UI/TextBlockWidget.h>

namespace GuGu {

	std::shared_ptr<AnchorDataCustomization> AnchorDataCustomization::create()
	{
		return std::make_shared<AnchorDataCustomization>();
	}

	void AnchorDataCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{

	}

	void AnchorDataCustomization::cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		customizeLayoutData(propertyHandle, childBuilder);
	}

	void AnchorDataCustomization::customizeLayoutData(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		customizeAnchors(propertyHandle, childBuilder);
	}

	void AnchorDataCustomization::customizeAnchors(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		std::shared_ptr<IPropertyHandle> anchorsHandle = propertyHandle->getChildHandle("m_anchors");

		IDetailPropertyRow& anchorsPropertyRow = childBuilder.addProperty(anchorsHandle);

		anchorsPropertyRow.customWidget(/*bShowChildren*/true)
		.nameContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text("114514")
		)
		.valueContent()
		(
			WIDGET_NEW(TextBlockWidget)
			.text("114514")
		);
	}

}