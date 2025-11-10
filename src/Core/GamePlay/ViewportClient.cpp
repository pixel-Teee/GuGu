#include <pch.h>

#include "ViewportClient.h"

#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/GameUI/UITransformComponent.h>
#include <Core/GamePlay/GameUI/UIComponent.h>
#include <Core/Collision/Collision3D.h>

namespace GuGu {
	ViewportClient::ViewportClient()
	{
		m_isInTerrainEditor = false;
		m_newTerrainBrushSize = 1.0f;
		m_newBrushStrength = 2.0f;
		m_brushType = BrushType::Increase;
	}
	ViewportClient::~ViewportClient()
	{
	}

	float ViewportClient::getDebugLineWidth()
	{
		return 10.0f;
	}

	GuGuUtf8Str ViewportClient::addGameObjectSelectionChangedEvent(std::function<void(const std::vector<GameObject*>&, bool)> inGameObjectSelectionChangedEvent)
	{
		//m_gameObjectSelectionChangedEvent = inGameObjectSelectionChangedEvent;
		return m_gameObjectSelectionChangedEvent.addDelegate(inGameObjectSelectionChangedEvent);
	}

	void ViewportClient::broadcastGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh)
	{
		m_gameObjectSelectionChangedEvent.invokeAll(newSelection, bForceRefresh);

		//for (const auto& it : m_gameObjectSelectionChangedEvents)
		//{
		//	if (it.m_event)
		//	{
		//		it.m_event(newSelection, bForceRefresh);
		//	}		
		//}
	}

	void ViewportClient::eraseGameObjectSelectionChangedEvent(const GuGuUtf8Str& inDelegateId)
	{
		m_gameObjectSelectionChangedEvent.remove(inDelegateId);
	}

	//void ViewportClient::addGameObjectSelectionChangedEvent(std::shared_ptr<GameObject> inGameObject, std::function<void(const std::vector<GameObject*>&, bool)> inGameObjectSelectionChangedEvent)
	//{
	//	m_gameObjectSelectionChangedEvents.push_back({ inGameObject, inGameObjectSelectionChangedEvent });
	//}
	//
	//void ViewportClient::eraseGameObjectSelectionChangedEvent(std::shared_ptr<GameObject> inGameObject)
	//{
	//	auto it = std::find_if(m_gameObjectSelectionChangedEvents.begin(),
	//		m_gameObjectSelectionChangedEvents.end(), [=](const TrackedEvent& item) {
	//			return item.gameObject.lock() == inGameObject;
	//		});
	//	if (m_gameObjectSelectionChangedEvents.end() != it)
	//	{
	//		m_gameObjectSelectionChangedEvents.erase(it);
	//	}
	//}

	//------input------
	void ViewportClient::onMouseButtonDown(UIPointerData uiPointerData)
	{
		GuGu_LOGD("trigger runtime ui mouse button down");

		//check runtime
		if (getViewportState() == ViewportState::Runtime)
		{
			std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
			Array<std::shared_ptr<GameObject>>& gameObjects = currentLevel->getGameObjects();

			math::float2 widthAndHeight;
			widthAndHeight = getViewportSize();

			math::float2 localScreenPosition;
			localScreenPosition = uiPointerData.getScreenPosition();

			//------ortho matrix------
			math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
			math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
			math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
			math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
			cameraUp = normalize(cross(cameraDir, cameraRight));
			math::affine3 viewMatrix = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
			math::float4x4 projMatrix = math::orthoProjD3DStyle(0, widthAndHeight.x, 0, widthAndHeight.y, 0, 1);
			//------ortho matrix------

			std::shared_ptr<GameObject> collidedUIGameObject = Collision3D::pickUIGameObject(localScreenPosition.x, localScreenPosition.y, widthAndHeight.x, widthAndHeight.y,
				projMatrix, math::affineToHomogeneous(viewMatrix), gameObjects);

			if (collidedUIGameObject)
			{
				//bubbling
				std::shared_ptr<GameObject> currentGameObject = collidedUIGameObject;
				while (currentGameObject)
				{
					std::shared_ptr<UIComponent> uiComponent = currentGameObject->getComponent<UIComponent>();
					if (uiComponent)
					{
						uiComponent->onPointerDown(uiPointerData);
					}
					currentGameObject = currentGameObject->getParentGameObject().lock();
				}
			}
		}	
	}

	void ViewportClient::onMouseButtonUp(UIPointerData uiPointerData)
	{
		GuGu_LOGD("trigger runtime ui mouse button up");

		//check runtime
		if (getViewportState() == ViewportState::Runtime)
		{
			std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
			Array<std::shared_ptr<GameObject>>& gameObjects = currentLevel->getGameObjects();

			math::float2 widthAndHeight;
			widthAndHeight = getViewportSize();

			math::float2 localScreenPosition;
			localScreenPosition = uiPointerData.getScreenPosition();

			//------ortho matrix------
			math::float3 cameraPos = math::float3(0.0f, 0.0f, 0.0f);
			math::float3 cameraDir = normalize(math::float3(0.0f, 0.0f, 1.0f));
			math::float3 cameraUp = math::float3(0.0f, 1.0f, 0.0f);
			math::float3 cameraRight = normalize(cross(cameraUp, cameraDir));
			cameraUp = normalize(cross(cameraDir, cameraRight));
			math::affine3 viewMatrix = math::affine3::from_cols(cameraRight, cameraUp, cameraDir, 0.0f);
			math::float4x4 projMatrix = math::orthoProjD3DStyle(0, widthAndHeight.x, 0, widthAndHeight.y, 0, 1);
			//------ortho matrix------

			std::shared_ptr<GameObject> collidedUIGameObject = Collision3D::pickUIGameObject(localScreenPosition.x, localScreenPosition.y, widthAndHeight.x, widthAndHeight.y,
				projMatrix, math::affineToHomogeneous(viewMatrix), gameObjects);

			if (collidedUIGameObject)
			{
				//bubbling
				std::shared_ptr<GameObject> currentGameObject = collidedUIGameObject;
				while (currentGameObject)
				{
					std::shared_ptr<UIComponent> uiComponent = currentGameObject->getComponent<UIComponent>();
					if (uiComponent)
					{
						uiComponent->onPointerUp(uiPointerData);
					}
					currentGameObject = currentGameObject->getParentGameObject().lock();
				}
			}
		}
	}

	void ViewportClient::setIsInTerrainEditor(bool isInTerrainEditor)
	{
		m_isInTerrainEditor = isInTerrainEditor;
	}

	bool ViewportClient::getIsInTerrainEditor() const
	{
		return m_isInTerrainEditor;
	}

	void ViewportClient::setBrushSize(float inNewTerrainBrushSize)
	{
		m_newTerrainBrushSize = inNewTerrainBrushSize;
	}

	void ViewportClient::setBrushStrength(float inNewBrushStrength)
	{
		m_newBrushStrength = inNewBrushStrength;
	}

	void ViewportClient::setBrushType(BrushType inNewBrushType)
	{
		m_brushType = inNewBrushType;
	}

	//------input------
}