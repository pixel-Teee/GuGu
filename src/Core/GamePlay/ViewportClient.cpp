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
	}
	ViewportClient::~ViewportClient()
	{
	}

	void ViewportClient::setGameObjectSelectionChangedEvent(std::function<void(const std::vector<GameObject*>&, bool)> inGameObjectSelectionChangedEvent)
	{
		m_gameObjectSelectionChangedEvent = inGameObjectSelectionChangedEvent;
	}

	void ViewportClient::broadcastGameObjectSelectionChanged(const std::vector<GameObject*>& newSelection, bool bForceRefresh)
	{
		m_gameObjectSelectionChangedEvent(newSelection, bForceRefresh);
	}

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
	//------input------
}