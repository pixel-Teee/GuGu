#include <pch.h>

#include "World.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "StaticMeshComponent.h"
#include "LightComponent.h"
#include "MaterialComponent.h"
#include "CameraComponent.h"
#include "Level.h"
#include "InputManager.h"

#include <Core/AssetManager/AssetData.h>
#include <Core/Model/StaticMesh.h>
#include <Core/AssetManager/AssetManager.h>
#include <Renderer/Demo.h>
#include <Core/GamePlay/ViewportClient.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/ScriptComponent.h>
#include <Application/Application.h>
#include <Core/Timer.h>
#include <Core/GamePlay/Prefab.h>

namespace GuGu {
	static bool registerGuGuWorld()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::World");
		auto& type = db.types[id];
		meta::TypeInfo<World>::Register(id, type, true, "1EC37ACB-19E4-4842-BDA4-25C312345107");

		auto typeID = typeidof(World);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<World>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<World>();

			type.AddConstructor<World, false, false>({});

			type.AddConstructor<World, true, true>({});

			type.LoadBaseClasses(db, typeID, { typeof(meta::Object) });

			meta::TypeInfo<World>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::World>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<World>>::Register(id, type, false, "76E5421B-ED24-4A01-8DFF-654DE5AD129A");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::World>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<World>>::Register(id, type, false, "0A299CC1-A26B-418B-9C55-770F9098BEC5");
		}


		return true;
	}

	//fields and functions
	static bool registerGuGuWorldFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(World).GetID()];

		type.AddField<Level, Array<std::shared_ptr<GameObject>>>("m_objects",
			(meta::FieldGetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::getGameObjects,
			(meta::FieldSetter<Level, Array<std::shared_ptr<GameObject>>&, true>::Signature) & Level::setGameObjects, {});

		//method functions
		std::shared_ptr<Level>(World::*getCurrentLevelPtr)() = &World::getCurrentLevel;
		type.AddMethod("getCurrentLevel", getCurrentLevelPtr, {});

		//static functions
		type.AddStaticMethod<World>("getWorld", &World::getWorld, {});

		type.AddMethod("loadTexture", &World::loadTexture, {});

		type.AddMethod("loadAnimation", &World::loadAnimation, {});

		type.AddMethod("getTotalTime", &World::getTotalTime, {});
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(World)
		ADD_PRIORITY(meta::Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuWorld)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(World)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_PRIORITY_FIELDS(Level)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuWorldFields)
	IMPLEMENT_INITIAL_FIELDS_END
	World::World()
	{
		m_editorLevel = std::make_shared<Level>();
		//create level
		m_currentLevel = m_editorLevel;
	}
	World::~World()
	{
	}
	std::shared_ptr<World>& World::getWorld()
	{
		static std::shared_ptr<World> g_world = std::make_shared<World>();
		return g_world;
	}

	std::shared_ptr<Level> World::getCurrentLevel()
	{
		return m_currentLevel;
	}

	const std::shared_ptr<Level> World::getCurrentLevel() const
	{
		return m_currentLevel;
	}
	void World::loadObject(AssetData& assetData) const
	{
		if (meta::Type::getType(assetData.m_assetTypeGuid) == meta::Type(meta::TypeIDs<GStaticMesh>::ID))
		{
			std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
			std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>();
			//transformComponent->SetScaling(0.2f);
			//transformComponent->SetTranslation(math::double3(5, 5, 0));
			//transformComponent->SetRotation(math::rotationQuat(math::double3(math::radians(60.0), math::radians(135.0), math::radians(40.0))));
			gameObject->addComponent(transformComponent);
			m_currentLevel->addGameObject(gameObject);

			GuGuUtf8Str filePath = assetData.m_filePath;
			AssetManager::getAssetManager().getRootFileSystem()->OpenFile(filePath.getStr(), GuGuFile::FileMode::OnlyRead);
			uint32_t fileSize = AssetManager::getAssetManager().getRootFileSystem()->getFileSize();
			char* fileContent = new char[fileSize + 1];
			fileContent[fileSize] = '\0';
			int32_t numberBytesHavedReaded = 0;
			AssetManager::getAssetManager().getRootFileSystem()->ReadFile((void*)fileContent, fileSize, numberBytesHavedReaded);
			AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
			GuGuUtf8Str modelJson(fileContent);
			//load model
			std::shared_ptr<GStaticMesh> gStaticMesh = AssetManager::getAssetManager().deserializeJson<GStaticMesh>(nlohmann::json::parse(modelJson.getStr()));
			assetData.m_loadedResource = std::static_pointer_cast<meta::Object>(gStaticMesh);
			//AssetManager::getAssetManager().deserializeJson(nlohmann::json::parse(modelJson.getStr()))
			
			delete[] fileContent;
			std::shared_ptr<StaticMeshComponent> staticMeshComponent = std::make_shared<StaticMeshComponent>();
			staticMeshComponent->setGStaticMesh(std::static_pointer_cast<AssetData>(std::shared_ptr<meta::Object>(assetData.Clone())));
			gameObject->addComponent(staticMeshComponent);

			std::shared_ptr<LightComponent> lightComponent = std::make_shared<LightComponent>();
			gameObject->addComponent(lightComponent);

			std::shared_ptr<MaterialComponent> materialComponent = std::make_shared<MaterialComponent>();
			gameObject->addComponent(materialComponent);


			//camera
			std::shared_ptr<GameObject> cameraGameObject = std::make_shared<GameObject>();
			std::shared_ptr<TransformComponent> cameraTransformComponent = std::make_shared<TransformComponent>();
			std::shared_ptr<CameraComponent> cameraComponent = std::make_shared<CameraComponent>();
			cameraTransformComponent->SetTranslation(math::double3(0.0f, 0.0f, -15.0f));
			cameraGameObject->addComponent(cameraTransformComponent);
			cameraGameObject->addComponent(cameraComponent);
			m_currentLevel->addGameObject(cameraGameObject);
		}
	}

	void World::loadLevel(AssetData& assetData)
	{
		if (meta::Type::getType(assetData.m_assetTypeGuid) == meta::Type(meta::TypeIDs<Level>::ID))
		{
			GuGuUtf8Str filePath = assetData.m_filePath;
			AssetManager::getAssetManager().getRootFileSystem()->OpenFile(filePath.getStr(), GuGuFile::FileMode::OnlyRead);
			uint32_t fileSize = AssetManager::getAssetManager().getRootFileSystem()->getFileSize();
			char* fileContent = new char[fileSize + 1];
			fileContent[fileSize] = '\0';
			int32_t numberBytesHavedReaded = 0;
			AssetManager::getAssetManager().getRootFileSystem()->ReadFile((void*)fileContent, fileSize, numberBytesHavedReaded);
			AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
			GuGuUtf8Str modelJson(fileContent);
			//load model
			std::shared_ptr<Level> level = std::shared_ptr<Level>(AssetManager::getAssetManager().deserializeJson<Level>(nlohmann::json::parse(modelJson.getStr())));
			assetData.m_loadedResource = std::static_pointer_cast<meta::Object>(level);
			//AssetManager::getAssetManager().deserializeJson(nlohmann::json::parse(modelJson.getStr()))

			delete[] fileContent;
			
			//m_currentLevel = level;
			m_editorLevel = level;
			m_viewportClient->setViewportState(ViewportClient::Editor);

			if (m_viewportClient->getViewportState() == ViewportClient::Runtime)
			{
				m_currentLevel = m_editorLevel;//todo:(clone)
				//start script
				Array<std::shared_ptr<GameObject>>& gameObjects = m_currentLevel->getGameObjects();
				for (int32_t i = 0; i < gameObjects.size(); ++i)
				{
					//check have script component
					std::shared_ptr<ScriptComponent> scriptComponent = gameObjects[i]->getComponent<ScriptComponent>();
					if (scriptComponent)
					{
						scriptComponent->initialize();
					}
				}
			}
		}
		for (uint32_t i = 0; i < World::getWorld()->m_onLevelChanged.size(); ++i)
		{
			World::getWorld()->m_onLevelChanged[i]();
		}
	}

	void World::loadPrefab(AssetData& assetData)
	{
		if (meta::Type::getType(assetData.m_assetTypeGuid) == meta::Type(meta::TypeIDs<Prefab>::ID))
		{
			GuGuUtf8Str filePath = assetData.m_filePath;
			AssetManager::getAssetManager().getRootFileSystem()->OpenFile(filePath.getStr(), GuGuFile::FileMode::OnlyRead);
			uint32_t fileSize = AssetManager::getAssetManager().getRootFileSystem()->getFileSize();
			char* fileContent = new char[fileSize + 1];
			fileContent[fileSize] = '\0';
			int32_t numberBytesHavedReaded = 0;
			AssetManager::getAssetManager().getRootFileSystem()->ReadFile((void*)fileContent, fileSize, numberBytesHavedReaded);
			AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
			GuGuUtf8Str prefablJson(fileContent);
			//load prefab
			std::shared_ptr<Prefab> prefab = std::shared_ptr<Prefab>(AssetManager::getAssetManager().deserializeJson<Prefab>(nlohmann::json::parse(prefablJson.getStr())));
			assetData.m_loadedResource = std::static_pointer_cast<meta::Object>(prefab);
			//AssetManager::getAssetManager().deserializeJson(nlohmann::json::parse(modelJson.getStr()))

			delete[] fileContent;

			std::shared_ptr<Prefab> instance = std::static_pointer_cast<Prefab>(AssetManager::getAssetManager().cloneObject(prefab));

			if (m_viewportClient->getViewportState() == ViewportClient::Editor)
			{
				//m_currentLevel->addGameObject();
				m_currentLevel->addGameObjects(instance->getGameObjects());
			}
		}
		//for (uint32_t i = 0; i < World::getWorld()->m_onLevelChanged.size(); ++i)
		//{
		//	World::getWorld()->m_onLevelChanged[i]();
		//}
	}

	std::shared_ptr<GuGu::AssetData> World::loadTexture(const GuGuUtf8Str& filePath)
	{
		return AssetManager::getAssetManager().loadAssetData(filePath);
	}

	std::shared_ptr<GuGu::AssetData> World::loadAnimation(const GuGuUtf8Str& filePath)
	{
		return AssetManager::getAssetManager().loadAssetData(filePath);
	}

	void World::setLevel(std::shared_ptr<Level> inLevel)
	{
		m_currentLevel = inLevel;
		for (uint32_t i = 0; i < World::getWorld()->m_onLevelChanged.size(); ++i)
		{
			World::getWorld()->m_onLevelChanged[i]();
		}
	}

	void World::update(float fElapsedTimeSeconds)
	{	
		getCurrentLevel()->Update(fElapsedTimeSeconds);
        if(m_viewportClient != nullptr)
		    m_viewportClient->update(fElapsedTimeSeconds);
	}

	void World::setViewportClient(std::shared_ptr<ViewportClient> viewportClient)
	{
		m_viewportClient = viewportClient;
	}

	std::weak_ptr<ViewportClient> World::getViewportClient() const
	{
		return m_viewportClient;
	}

	void World::renderLevel(Demo* demoPass)
	{
		if (m_viewportClient != nullptr)
		{
			if (m_viewportClient->getViewportState() == ViewportClient::Editor)
			{
				if (demoPass && m_viewportClient != nullptr)
				{
					if (m_viewportClient->getRenderTarget() == nullptr || math::any(m_viewportClient->getViewportSize() != m_viewportClient->getRenderTargetSize()))
					{
						demoPass->initRenderTargetAndDepthTarget(*m_viewportClient, m_viewportClient->getViewportSize());
					}

					demoPass->renderLevel(m_currentLevel, m_viewportClient);
				}
			}
			else
			{
				if (demoPass && m_viewportClient != nullptr)
				{
					if (m_viewportClient->getRenderTarget() == nullptr || math::any(m_viewportClient->getViewportSize() != m_viewportClient->getRenderTargetSize()))
					{
						demoPass->initRenderTargetAndDepthTarget(*m_viewportClient, m_viewportClient->getViewportSize());
					}

					demoPass->renderLevelRuntime(m_currentLevel, m_viewportClient);
				}
			}
		}	
	}

	void World::switchState(WorldState state)
	{
		if (state == ViewportClient::ViewportState::Runtime)
		{
			//m_currentLevel = std::shared_ptr<Level>(static_cast<Level*>(static_cast<meta::Object*>(m_editorLevel->Clone())));
			m_currentLevel = std::static_pointer_cast<Level>(AssetManager::getAssetManager().cloneObject(m_editorLevel));

			//start script
			Array<std::shared_ptr<GameObject>>& gameObjects = m_currentLevel->getGameObjects();
			for (int32_t i = 0; i < gameObjects.size(); ++i)
			{
				//check have script component
				std::shared_ptr<ScriptComponent> scriptComponent = gameObjects[i]->getComponent<ScriptComponent>();
				if (scriptComponent)
				{
					scriptComponent->initialize();
				}
			}
		}
		else if(state == ViewportClient::ViewportState::Editor)
		{
			m_currentLevel = m_editorLevel;

			//close script
			Array<std::shared_ptr<GameObject>>& gameObjects = m_currentLevel->getGameObjects();
			for (int32_t i = 0; i < gameObjects.size(); ++i)
			{
				//check have script component
				std::shared_ptr<ScriptComponent> scriptComponent = gameObjects[i]->getComponent<ScriptComponent>();
				if (scriptComponent)
				{
					scriptComponent->close();
				}
			}
		}
	}

	void World::Update(float fElapsedTimeSeconds)
	{
		//nothing
	}

	meta::Type World::GetType() const
	{
		return typeof(World);
	}

	meta::Object* World::Clone(void) const
	{
		//nothing
		return nullptr;
	}

	void World::PostLoad()
	{
		//nothing
	}

	float World::getTotalTime() const
	{
		return Application::getApplication()->getTimer()->GetTotalTime();
	}

}