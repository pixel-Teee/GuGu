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
#include <Core/UI/Slider.h>
#include <Core/UI/Box.h>
#include <Core/UI/ListView.h>
#include <Core/UI/ComboBox.h>

#include <Core/AssetManager/AssetManager.h>
#include <Core/GamePlay/Prefab.h>
#include <Core/GamePlay/TerrainVegetationComponent.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/GamePlay/Level.h>

#include <Editor/StyleSet/EditorStyleSet.h>
#include <Editor/PropertyEditor/ObjectDetails.h>
#include <Editor/ContentBrowser/ContentBrowserSingleton.h>

namespace GuGu {

	TerrainEditorPanel::TerrainEditorPanel()
	{
		m_brushType = 0;
	}

	TerrainEditorPanel::~TerrainEditorPanel()
	{
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		viewportClient->eraseGameObjectSelectionChangedEvent(m_gameObjectSelectionChangedDelegateHandle);
		viewportClient->setIsInTerrainEditor(false);
		viewportClient->setIsInVegetationPaint(false);
	}

	void TerrainEditorPanel::init(const BuilderArguments& arguments)
	{
		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		//std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();

		std::shared_ptr<Button> sculptButton;
		std::shared_ptr<Button> paintButton;

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
					+ HorizontalBox::Slot()
					.setPadding(Padding(10.0f, 0.0f, 0.0f, 0.0f)) //left padding
					.FixedWidth()
					(
						WIDGET_ASSIGN_NEW(Button, paintButton)
						.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
						.Content
						(
							WIDGET_NEW(TextBlockWidget)
							.text(u8"paint")
							.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
						)
					)
				)
				+ VerticalBox::Slot()
				.FixedHeight()
				(
					WIDGET_ASSIGN_NEW(Border, m_brushContent)
					.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
						return EditorStyleSet::getStyleSet()->getColor("ColorLevel4");
					}))
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
		viewportClient->setBrushSize(1.0f);//brush size

		m_visibilityAttribute = Visibility::Visible;

		sculptButton->setOnClicked(OnClicked(std::bind(&TerrainEditorPanel::showPanelContent, this, TerrainEditorContentType::Scuplt)));
		paintButton->setOnClicked(OnClicked(std::bind(&TerrainEditorPanel::showPanelContent, this, TerrainEditorContentType::Paint)));

		AssetPickerConfig assetPickerConfig;
		assetPickerConfig.m_initialAssetViewType = AssetViewType::List;
		assetPickerConfig.m_onAssetSelected = std::bind(&TerrainEditorPanel::onPaintAssetSelected, this, std::placeholders::_1);
		assetPickerConfig.m_filter.m_classNames.push_back("GuGu::Prefab");

		m_paintPanel = 
			WIDGET_NEW(VerticalBox)
			+ VerticalBox::Slot()
			.FixedHeight()
			(
				createAssetPicker(assetPickerConfig)
			);

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

		m_sculptVerticalBox->addSlot()
		.FixedHeight()
		(
			WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(TextBlockWidget)
				.text(u8"Brush Size")
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
			)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(BoxWidget)
				.MinDesiredWidth(200.0f)
				.Content
				(
					WIDGET_NEW(Slider)
					.visibility(Visibility::Visible)
					.MinValue(1.0f)
					.MaxValue(10.0f) //max brush size
					.OnValueChanged(this, &TerrainEditorPanel::onBrushSizeChanged)
					.Value(this, &TerrainEditorPanel::getBrushSize)
				)
			)
		);

		m_sculptVerticalBox->addSlot()
		.FixedHeight()
		(
			WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(TextBlockWidget)
				.text(u8"Brush Strength")
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
			)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(BoxWidget)
				.MinDesiredWidth(200.0f)
				.Content
				(
					WIDGET_NEW(Slider)
					.visibility(Visibility::Visible)
					.MinValue(1.0f)
					.MaxValue(10.0f) //max brush size
					.OnValueChanged(this, &TerrainEditorPanel::onBrushStrengthChanged)
					.Value(this, &TerrainEditorPanel::getBrushStrength)
				)
			)
		);

		m_brushTypeStrSources.push_back("Increase");
		m_brushTypeStrSources.push_back("Decrease");

		m_sculptVerticalBox->addSlot()
		.FixedHeight()
		(
			WIDGET_NEW(HorizontalBox)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(TextBlockWidget)
				.text(u8"Brush Type")
				.textColor(Attribute<math::float4>::Create([=]() {
					return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
				}))
			)
			+ HorizontalBox::Slot()
			.FixedWidth()
			(
				WIDGET_NEW(ComboBox<GuGuUtf8Str>)
				.visibility(Visibility::Visible)
				.optionSource(&m_brushTypeStrSources)
				.onGenerateWidgetLambda([&](GuGuUtf8Str inOption)->std::shared_ptr<Widget> {
					return WIDGET_NEW(TextBlockWidget)
						.text(inOption)
						.textColor(Attribute<math::float4>::Create([=]() {
						return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
					}));
				})
				.onSelectionChanged(this, &TerrainEditorPanel::onBrushTypeSelect)
				//.maxListHeight(60.0f)
				.Content
				(
					WIDGET_NEW(Border)
					.BorderBackgroundColor(Attribute<math::float4>::Create([=]() {
						return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel4");
					}))
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.text(Attribute<GuGuUtf8Str>::Create(std::bind(&TerrainEditorPanel::getBrushTypeStr, this)))
						.textColor(Attribute<math::float4>::Create([=]() {
							return EditorStyleSet::getStyleSet()->getColor("SecondaryColorLevel9");
						}))
					)
				)
				.initiallySelectedItem(u8"Increase")
			)
		);
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
			m_brushContent->setContent(m_paintPanel);
			std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
			viewportClient->m_paintPositionCallback = std::bind(&TerrainEditorPanel::paintVegetation, this, std::placeholders::_1);
			viewportClient->setIsInTerrainEditor(true);
			viewportClient->setIsInVegetationPaint(true);
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

	float TerrainEditorPanel::getBrushSize() const
	{
		return m_brushSize;
	}

	void TerrainEditorPanel::onBrushSizeChanged(float inNewBrushSize)
	{
		m_brushSize = inNewBrushSize;
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		if (viewportClient)
		{
			viewportClient->setBrushSize(inNewBrushSize);
		}
	}

	float TerrainEditorPanel::getBrushStrength() const
	{
		return m_brushStrength;
	}

	void TerrainEditorPanel::onBrushStrengthChanged(float inNewBrushStrength)
	{
		m_brushStrength = inNewBrushStrength;
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		if (viewportClient)
		{
			viewportClient->setBrushStrength(inNewBrushStrength);
		}
	}

	void TerrainEditorPanel::onBrushTypeSelect(GuGuUtf8Str inOption, SelectInfo::Type)
	{
		if(inOption == "Increase")
			m_brushType = 0;
		else if(inOption == "Decrease")
			m_brushType = 1;
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		if (viewportClient)
		{
			viewportClient->setBrushType((ViewportClient::BrushType)m_brushType);
		}
	}

	GuGu::GuGuUtf8Str TerrainEditorPanel::getBrushTypeStr() const
	{
		return m_brushTypeStrSources[m_brushType];
	}

	void TerrainEditorPanel::onPaintAssetSelected(const AssetData& inAsset)
	{
		m_paintVegetation = inAsset;
	}

	void TerrainEditorPanel::paintVegetation(math::float3 inBrushPos)
	{
		if (m_paintVegetation.m_filePath != "" && m_terrainObject.lock() != nullptr)
		{
			if (meta::Type::getType(m_paintVegetation.m_assetTypeGuid).GetID() == meta::TypeIDs<Prefab>::ID)
			{
				std::shared_ptr<AssetData> loadedAssetData = AssetManager::getAssetManager().loadPrefab(m_paintVegetation.m_filePath);
				if (loadedAssetData)
				{
					//find root
					std::shared_ptr<Prefab> instancePrefab = std::static_pointer_cast<Prefab>(loadedAssetData->m_loadedResource);

					std::shared_ptr<GameObject> rootGameObj;
					Array<std::shared_ptr<GameObject>> objs = instancePrefab->getGameObjects();
					for (int32_t i = 0; i < objs.size(); ++i)
					{
						if (objs[i]->getParentGameObject().lock() == nullptr)
							rootGameObj = objs[i];
					}
					if (rootGameObj)
					{
						std::shared_ptr<GameObject> gameObj = std::static_pointer_cast<GameObject>(AssetManager::getAssetManager().cloneObject(rootGameObj));
						if (gameObj)
						{
							std::shared_ptr<TerrainComponent> terrainComponent = m_terrainObject.lock()->getComponent<TerrainComponent>();
							if (terrainComponent)
							{
								gameObj->traverseGameObjectTrees([=](std::shared_ptr<GameObject> inGameObj) {
									//paint
									std::shared_ptr<TerrainVegetationComponent> terrainVegetationComp = std::static_pointer_cast<TerrainVegetationComponent>(inGameObj->getComponent<TerrainVegetationComponent>());
									if (terrainVegetationComp && m_terrainObject.lock())
									{
										if (terrainComponent)
										{
											terrainVegetationComp->setTerrainOwner(terrainComponent);
										}
									}
									else
									{
										terrainVegetationComp = std::make_shared<TerrainVegetationComponent>();
										inGameObj->addComponent(terrainVegetationComp);
										terrainVegetationComp->setTerrainOwner(terrainComponent);
									}
									//add to current level
									std::shared_ptr<World> currentWorld = World::getWorld();
									if (currentWorld)
									{
										currentWorld->getCurrentLevel()->addGameObject(inGameObj);
									}
								});	
							}
							std::shared_ptr<TransformComponent> transformComponent = gameObj->getComponent<TransformComponent>();
							if (transformComponent)
							{
								transformComponent->SetTranslation(math::double3(inBrushPos));
							}
						}
					}			
				}
			}
		}	
	}

}