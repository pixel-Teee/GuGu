#include <pch.h>

#include "PropertyEditorAsset.h"
#include "PropertyNode.h"
#include "PropertyEditor.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/AssetManager.h>
#include <Core/AssetManager/GameObjectLevelRef.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Box.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/Level.h>

#include <Editor/StyleSet/EditorStyleSet.h>

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
		if (nodeProperty->GetType() == typeof(std::shared_ptr<GameObjectLevelRef>)) //game object
			return true;
		return false;
	}

	PropertyEditorAsset::PropertyEditorAsset()
	{
		m_isGameObjectLevelRef = false;
	}

	PropertyEditorAsset::~PropertyEditorAsset()
	{

	}

	void PropertyEditorAsset::init(const BuilderArguments& arguments, const std::shared_ptr<PropertyEditor>& inPropertyEditor)
	{
		m_propertyEditor = inPropertyEditor;
		m_propertyHandle = arguments.mpropertyHandle;
		m_ownerAssetDataArray = arguments.mownerAssetDataArray;

		if (inPropertyEditor)
		{
			AssetData assetData;
			inPropertyEditor->getPropertyHandle()->getValue(assetData);
			m_objectClass = meta::Type::getType(assetData.m_assetTypeGuid);
			m_allowedClassFilters.push_back(m_objectClass);
		}

		if (inPropertyEditor)
		{
			const meta::Field* field = inPropertyEditor->getField();
			if(field)
				m_isGameObjectLevelRef = (field->GetType() == typeof(std::shared_ptr<GameObjectLevelRef>));
		}

		m_assetComboButton = WIDGET_NEW(ComboButton)
		.onGetMenuContent(this, &PropertyEditorAsset::onGetMenuContent)
		.method(std::optional<PopupMethod>(PopupMethod::CreateNewWindow))
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
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
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
		if (m_isGameObjectLevelRef)
		{
			GameObjectLevelRef value;
			getValue(value);

			OnGameObjectSelected gameObjectSelected = std::bind(&PropertyEditorAsset::onGameObjectSelected, this, std::placeholders::_1);
			return PropertyCustomizationHelpers::makeGameObjectPickerWithMenu(value, gameObjectSelected);
		}
		else
		{
			AssetData value;
			getValue(value);

			//return PropertyCustomizationHelpers::makeAssetPickerWithMenu(m_allowedClassFilters);
			OnAssetSelected assetSelected = std::bind(&PropertyEditorAsset::onAssetSelected, this, std::placeholders::_1);
			return PropertyCustomizationHelpers::makeAssetPickerWithMenu(value, m_allowedClassFilters, assetSelected);
		}	
	}

	std::shared_ptr<Brush> PropertyEditorAsset::getStatusIcon() const
	{
		return CoreStyle::getStyleSet()->getBrush("NoBorder");
	}

	GuGuUtf8Str PropertyEditorAsset::onGetAssetName() const
	{
		GuGuUtf8Str resultName;
		if (m_isGameObjectLevelRef)
		{
			GameObjectLevelRef objLevelRef;
			getValue(objLevelRef);
			resultName = objLevelRef.m_gameObjectName;
		}
		else
		{
			AssetData assetData;
			getValue(assetData);

			GuGuUtf8Str baseName = assetData.m_fileName;
			if (baseName.findLastOf(".") != -1)
			{
				baseName = baseName.substr(0, baseName.findLastOf("."));
			}
			resultName = baseName;
		}

		return resultName;
	}

	void PropertyEditorAsset::getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
	{
		outMinFixedWidth = 250.0f;
		outMaxFixedWidth = 350.0f;
	}

	PropertyAccess::Result PropertyEditorAsset::getValue(AssetData& outValue) const
	{
		PropertyAccess::Result result = PropertyAccess::Result::Fail;
		//获取资产路径
		if (m_propertyEditor)
		{
			result = m_propertyEditor->getPropertyHandle()->getValue(outValue);
		}
		return result;
	}

	PropertyAccess::Result PropertyEditorAsset::getValue(GameObjectLevelRef& outValue) const
	{
		PropertyAccess::Result result = PropertyAccess::Result::Fail;
		//获取资产路径
		if (m_propertyEditor)
		{
			result = m_propertyEditor->getPropertyHandle()->getValue(outValue);
		}
		return result;
	}

	void PropertyEditorAsset::onAssetSelected(const AssetData& assetData)
	{
		setValue(assetData);
	}

	void PropertyEditorAsset::onGameObjectSelected(std::shared_ptr<GameObject> inGameObject)
	{
		std::shared_ptr<Level> currentLevel = inGameObject->getCurrentLevel();
		std::shared_ptr<AssetData> assetData = AssetManager::getAssetManager().getAssetData(currentLevel);
		
		GameObjectLevelRef gameObjectLevelRef;
		gameObjectLevelRef.m_levelFilePath = assetData->m_filePath;
		gameObjectLevelRef.m_levelFileName = assetData->m_fileName;
		gameObjectLevelRef.m_gameObjectName = inGameObject->getName();

		setValue(gameObjectLevelRef);
	}

	void PropertyEditorAsset::setValue(const AssetData& assetData)
	{
		m_assetComboButton->setIsOpen(false);

		if (m_propertyEditor)
		{
			m_propertyEditor->getPropertyHandle()->setValue(assetData);
		}
	}

	void PropertyEditorAsset::setValue(const GameObjectLevelRef& inGameObjectLevelRef)
	{
		m_assetComboButton->setIsOpen(false);

		if (m_propertyEditor)
		{
			m_propertyEditor->getPropertyHandle()->setValue(inGameObjectLevelRef);
		}
	}

}