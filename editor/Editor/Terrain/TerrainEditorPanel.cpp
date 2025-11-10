#include <pch.h>

#include <Core/GamePlay/TerrainComponent.h>
#include "TerrainEditorPanel.h"

#include <Core/GamePlay/World.h>
#include <Core/GamePlay/ViewportClient.h>
#include <Core/GamePlay/GameObject.h>

#include <Core/UI/Overlay.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/CompoundWidget.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Border.h>
#include <Core/UI/Button.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/NumericEntryBox.h>
#include <Core/UI/Reply.h>

#include <Editor/StyleSet/EditorStyleSet.h>


namespace GuGu {

	TerrainEditorPanel::TerrainEditorPanel()
	{

	}

	TerrainEditorPanel::~TerrainEditorPanel()
	{
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		viewportClient->eraseGameObjectSelectionChangedEvent(m_gameObjectSelectionChangedDelegateHandle);
		viewportClient->setIsInTerrainEditor(false);
	}

	void TerrainEditorPanel::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		//std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();

		std::shared_ptr<Button> sculptButton;

		std::shared_ptr<Overlay> overlay = 
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				//brush type
				WIDGET_NEW(VerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(HorizontalBox, m_brushSelectButtons)
					+ HorizontalBox::Slot()
					.FixedWidth()
					(
						WIDGET_ASSIGN_NEW(Button, sculptButton)
						.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
						.Content
						(
							WIDGET_NEW(TextBlockWidget)
							.text(u8"sculpt")
							.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
						)
					)
				)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Border, m_brushContent)
					.Content
					(
						NullWidget::getNullWidget()
					)
				)
			);
		m_childWidget->m_childWidget = overlay;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();

		std::function<void(const std::vector<GameObject*>&, bool)> registerFunc = std::bind(&TerrainEditorPanel::onGameObjectSelectionChanged, this, std::placeholders::_1, std::placeholders::_2);
		m_gameObjectSelectionChangedDelegateHandle = viewportClient->addGameObjectSelectionChangedEvent(registerFunc);

		m_visibilityAttribute = Visibility::Visible;

		sculptButton->setOnClicked(OnClicked(std::bind(&TerrainEditorPanel::showPanelContent, this, TerrainEditorContentType::Scuplt)));

		m_sculptOverlay = 
			WIDGET_NEW(Overlay)
			+ Overlay::Slot()
			(
				WIDGET_ASSIGN_NEW(VerticalBox, m_sculptVerticalBox)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					NullWidget::getNullWidget()
				)
			);

		std::vector<GuGuUtf8Str> variableNames = {
			"m_terrainRows",
			"m_terrainCols",
			"m_rows",
			"m_cols",
			"m_tileSize",
			"m_heightScale"
		};

		for (int32_t i = 0; i < variableNames.size(); ++i)
		{
			//loop and variable name
			m_sculptVerticalBox->addSlot()
			.FixedHeight()
			(
				WIDGET_NEW(HorizontalBox)
				+ HorizontalBox::Slot()
				.FixedWidth()
				(
					WIDGET_NEW(TextBlockWidget)
					.text(variableNames[i].getStr())
					.textColor(Attribute<math::float4>::Create([=]() {
						return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
					}))
				)
				+ HorizontalBox::Slot()
				.FixedWidth()
				(
					WIDGET_NEW(NumericEntryBox<int32_t>)
					.allowSpain(true)
					.value(Attribute<std::optional<int32_t>>::Create(std::bind(&TerrainEditorPanel::onGetValue, this, variableNames[i]))) //要显示的值
					.onValueChanged(this, &TerrainEditorPanel::onValueChanged, variableNames[i])
					.onValueCommitted(this, &TerrainEditorPanel::onValueCommitted, variableNames[i])
					.minValue(0)
					.maxValue(1000)
				)
			);
		}	
		std::shared_ptr<Button> recreateTerrainButton;
		m_sculptVerticalBox->addSlot()
		.FixedHeight()
		(
			WIDGET_ASSIGN_NEW(Button, recreateTerrainButton)
			.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
			.Content
			(
				WIDGET_NEW(HorizontalBox)
				//+ HorizontalBox::Slot()
				//.FixedWidth()
				//(
				//	WIDGET_NEW(ImageWidget)
				//	.brush(EditorStyleSet::getStyleSet()->getBrush("ImportModel_Icon"))
				//)
				+ HorizontalBox::Slot()
				.setPadding(Padding(5.0f, 0.0f, 5.0f, 0.0f))
				.FixedWidth()
				(
					WIDGET_NEW(TextBlockWidget)
					.text(u8"RecreateButton")
					.textColor(Attribute<math::float4>::Create([=]() {
						return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
					}))
				)	
			)
		);

		recreateTerrainButton->setOnClicked(OnClicked(std::bind(&TerrainEditorPanel::recreateTerrain, this)));
	}

	void TerrainEditorPanel::onGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh)
	{
		//find first terrain
		for (int32_t i = 0; i < newSelection.size(); ++i)
		{
			if (newSelection[i]->shared_from_this())
			{
				std::shared_ptr<GameObject> lockedGameObject = std::static_pointer_cast<GameObject>(newSelection[i]->shared_from_this());
				if (lockedGameObject)
				{
					if (lockedGameObject->getComponent<TerrainComponent>())
					{
						m_terrainObject = lockedGameObject;
						break;
					}
				}
			}
		}
		if (m_terrainObject.lock() == nullptr)
		{
			m_brushContent->setContent(NullWidget::getNullWidget());
		}
		else
		{
			std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
			viewportClient->setIsInTerrainEditor(true);
			//m_brushContent->setContent();
			showPanelContent(TerrainEditorContentType::Scuplt);
		}
	}

	Reply TerrainEditorPanel::showPanelContent(TerrainEditorContentType inContentType)
	{
		if (inContentType == TerrainEditorContentType::Scuplt)
		{
			//terrain info
			m_brushContent->setContent(m_sculptOverlay);
		}
		else if (inContentType == TerrainEditorContentType::Paint)
		{

		}
		return Reply::Handled();
	}

	std::optional<int32_t> TerrainEditorPanel::onGetValue(const GuGuUtf8Str& variableName) const
	{
		if (m_terrainObject.lock())
		{
			std::shared_ptr<TerrainComponent> terrainComponent = m_terrainObject.lock()->getComponent<TerrainComponent>();
			if (terrainComponent)
			{
				meta::Type type = terrainComponent->GetType();
				//get field
				meta::Field field = meta::ReflectionDatabase::Instance().types[type.GetID()].GetField(variableName.getStr());
				//return field.GetValue(ObjectVariant(terrainComponent.get()));
				meta::Variant instance = ObjectVariant(terrainComponent.get());
				meta::Variant value = field.GetValue(instance);

				//if(field.GetType() == typeof(uint32_t))
				//	return value.GetValue<uint32_t>();
				//else if(field.GetType() == typeof(float))
				//	return value.GetValue<float>();

				auto it = m_cacheVariableValues.find(variableName);

				if (it != m_cacheVariableValues.end())
				{
					if (field.GetType() == typeof(uint32_t))
						return it->second.GetValue<uint32_t>();
					else if (field.GetType() == typeof(float))
						return it->second.GetValue<float>();
				}
				else
				{
					if(field.GetType() == typeof(uint32_t))
						return value.GetValue<uint32_t>();
					else if(field.GetType() == typeof(float))
						return value.GetValue<float>();

				}
			}
		}
		return std::optional<int32_t>(0);
	}

	void TerrainEditorPanel::onValueChanged(int32_t variableValue, GuGuUtf8Str variableName) 
	{
		if (m_terrainObject.lock())
		{
			std::shared_ptr<TerrainComponent> terrainComponent = m_terrainObject.lock()->getComponent<TerrainComponent>();
			if (terrainComponent)
			{
				meta::Type type = terrainComponent->GetType();
				//get field
				meta::Field field = meta::ReflectionDatabase::Instance().types[type.GetID()].GetField(variableName.getStr());
				//return field.GetValue(ObjectVariant(terrainComponent.get()));
				meta::Variant instance = ObjectVariant(terrainComponent.get());
				meta::Variant value;
				if (field.GetType() == typeof(uint32_t))
					value = (uint32_t)variableValue;
				else if (field.GetType() == typeof(float))
					value = (float)variableValue;
				//field.SetValue(instance, value);

				m_cacheVariableValues[field.GetName()] = std::move(value);
			}
		}
	}

	void TerrainEditorPanel::onValueCommitted(int32_t variableValue, TextCommit::Type commitType, GuGuUtf8Str variableName)
	{
		if (m_terrainObject.lock())
		{
			std::shared_ptr<TerrainComponent> terrainComponent = m_terrainObject.lock()->getComponent<TerrainComponent>();
			if (terrainComponent)
			{
				meta::Type type = terrainComponent->GetType();
				//get field
				meta::Field field = meta::ReflectionDatabase::Instance().types[type.GetID()].GetField(variableName.getStr());
				//return field.GetValue(ObjectVariant(terrainComponent.get()));
				meta::Variant instance = ObjectVariant(terrainComponent.get());
				meta::Variant value;
				if (field.GetType() == typeof(uint32_t))
					value = (uint32_t)variableValue;
				else if (field.GetType() == typeof(float))
					value = (float)variableValue;
				//field.SetValue(instance, value);
				m_cacheVariableValues[field.GetName()] = std::move(value);
			}
		}
	}

	Reply TerrainEditorPanel::recreateTerrain()
	{
		if (m_terrainObject.lock())
		{
			std::shared_ptr<TerrainComponent> terrainComponent = m_terrainObject.lock()->getComponent<TerrainComponent>();
			meta::Type terrainType = terrainComponent->GetType();
			meta::Variant instance = ObjectVariant(terrainComponent.get());
			for (const auto& item : m_cacheVariableValues)
			{
				meta::Field field = meta::ReflectionDatabase::Instance().types[terrainType.GetID()].GetField(item.first.getStr());
				//return field.GetValue(ObjectVariant(terrainComponent.get()));
				field.SetValue(instance, item.second);
			}
			if (terrainComponent)
			{
				terrainComponent->createTileData();
			}
		}
		return Reply::Handled();
	}

}