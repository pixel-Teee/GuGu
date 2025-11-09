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

#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {

	TerrainEditorPanel::TerrainEditorPanel()
	{

	}

	TerrainEditorPanel::~TerrainEditorPanel()
	{
		std::shared_ptr<ViewportClient> viewportClient = World::getWorld()->getViewportClient().lock();
		viewportClient->eraseGameObjectSelectionChangedEvent(m_gameObjectSelectionChangedDelegateHandle);
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
			//m_brushContent->setContent();
			showPanelContent(TerrainEditorContentType::Scuplt);
		}
	}

	Reply TerrainEditorPanel::showPanelContent(TerrainEditorContentType inContentType)
	{
		if (inContentType == TerrainEditorContentType::Scuplt)
		{

		}
		else if (inContentType == TerrainEditorContentType::Paint)
		{

		}
		return Reply::Handled();
	}

}