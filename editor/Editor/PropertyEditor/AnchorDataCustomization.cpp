#include <pch.h>

#include "AnchorDataCustomization.h"
#include "PropertyHandleImp.h"
#include "DetailPropertyRow.h"
#include "CustomChildBuilder.h"//IDetailChildrenBuilder
#include <Editor/StyleSet/EditorStyleSet.h>

#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/Border.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/UniformGridPanel.h>
#include <Core/UI/CompoundWidget.h>

#include <Core/GamePlay/GameUI/UIAnchors.h>

namespace GuGu {

	class AnchorPreviewWidget : public CompoundWidget
	{
	public:
		AnchorPreviewWidget() {}
		virtual ~AnchorPreviewWidget() {}

		struct BuilderArguments : public Arguments<AnchorPreviewWidget>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, 
		std::shared_ptr<IPropertyHandle> anchorsHandle,
		std::shared_ptr<IPropertyHandle> alignmentHandle,
		std::shared_ptr<IPropertyHandle> offsetsHandle,
		const GuGuUtf8Str& labelText,
		UIAnchors anchors)
		{
			//
		}

	};

	std::shared_ptr<AnchorDataCustomization> AnchorDataCustomization::create()
	{
		return std::make_shared<AnchorDataCustomization>();
	}

	void AnchorDataCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		//headerRow
		//.nameContent()
		//(
		//	WIDGET_NEW(TextBlockWidget)
		//	.text("114514")
		//)
		//.valueContent()
		//(
		//	WIDGET_NEW(TextBlockWidget)
		//	.text("114514")
		//);
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
			.text("Anchors")
			.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
		)
		.valueContent()
		(
			WIDGET_NEW(ComboButton)
			.method(std::optional<PopupMethod>(PopupMethod::CreateNewWindow))
			.buttonContent
			(
				WIDGET_NEW(TextBlockWidget)
				.text("Anchors")
				.textColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
			)
			.menuContent
			(
				WIDGET_NEW(Border)
				.padding(5)
				.Content
				(
					WIDGET_NEW(Border)
					.BorderBackgroundColor(EditorStyleSet::getStyleSet()->getColor("beige9"))
					.Content
					(
						WIDGET_NEW(VerticalBox)
						+ VerticalBox::Slot()
						.FixedHeight()
						(
							WIDGET_NEW(HorizontalBox)
							+ HorizontalBox::Slot()
							.FixedWidth()
							(
								WIDGET_NEW(UniformGridPanel)
								+ UniformGridPanel::Slot(0, 0)
								(
									NullWidget::getNullWidget()
								)
							)
						)
					)
				)
			)
		);
	}

}