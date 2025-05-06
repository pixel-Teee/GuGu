#include <pch.h>

#include "PropertyEditorAsset.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include <Core/AssetManager/AssetData.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Box.h>

namespace GuGu {

	bool PropertyEditorAsset::supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();

		return supports(propertyNode->getField());
	}

	bool PropertyEditorAsset::supports(const meta::Field* nodeProperty)
	{
		if (nodeProperty->GetType() == typeof(std::shared_ptr<AssetData>))
			return true;
		return false;
	}

	PropertyEditorAsset::PropertyEditorAsset()
	{

	}

	PropertyEditorAsset::~PropertyEditorAsset()
	{

	}

	void PropertyEditorAsset::init(const BuilderArguments& arguments, const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;
		m_propertyHandle = arguments.mpropertyHandle;
		m_ownerAssetDataArray = arguments.mownerAssetDataArray;

		m_assetComboButton = WIDGET_NEW(ComboButton)
		.onGetMenuContent(this, &PropertyEditorAsset::onGetMenuContent)
		.buttonContent
		(
			WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.FixedWidth()
			.setHorizontalAlignment(HorizontalAlignment::Left)
			.setVerticalAlignment(VerticalAlignment::Center)
			(
				WIDGET_NEW(ImageWidget)
				.brush(this, &PropertyEditorAsset::getStatusIcon)
			)
			+ HorizontalBox::Slot()
			.StretchWidth(1.0f)
			.setVerticalAlignment(VerticalAlignment::Center)
			(
				WIDGET_NEW(TextBlockWidget)
				.text(this, &PropertyEditorAsset::onGetAssetName)
			)
		);

		std::shared_ptr<Widget> buttonBoxWrapper;
		std::shared_ptr<HorizontalBox> valueContentBox = WIDGET_NEW(HorizontalBox);

		valueContentBox->addSlot()
		.setHorizontalAlignment(HorizontalAlignment::Center)
		.FixedWidth()
		(
			WIDGET_NEW(VerticalBox)
			+ VerticalBox::Slot()
			.FixedHeight()
			.setVerticalAlignment(VerticalAlignment::Center)
			(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.FixedWidth()
				(
					m_assetComboButton
				)
				//+ HorizontalBox::Slot()
				//(
				//	WIDGET_ASSIGN_NEW(BoxWidget, buttonBoxWrapper)
				//	.padding(Padding(4.0f, 0.0f))
				//	.Content
				//	(
				//		
				//	)
				//)
			)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = valueContentBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

	std::shared_ptr<Widget> PropertyEditorAsset::onGetMenuContent()
	{
		return NullWidget::getNullWidget();
	}

	std::shared_ptr<Brush> PropertyEditorAsset::getStatusIcon() const
	{
		return CoreStyle::getStyleSet()->getBrush("NoBorder");
	}

	GuGuUtf8Str PropertyEditorAsset::onGetAssetName() const
	{
		return u8"kksk";
	}

	void PropertyEditorAsset::getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
	{
		outMinFixedWidth = 250.0f;
		outMaxFixedWidth = 350.0f;
	}

}