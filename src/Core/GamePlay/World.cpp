#include <pch.h>

#include "World.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "StaticMeshComponent.h"
#include "Level.h"

#include <Core/AssetManager/AssetData.h>
#include <Core/Model/StaticMesh.h>
#include <Core/AssetManager/AssetManager.h>

namespace GuGu {
	World::World()
	{
		//create level
		m_currentLevel = std::make_shared<Level>();		
	}
	World::~World()
	{
	}
	std::shared_ptr<World>& World::getWorld()
	{
		static std::shared_ptr<World>& g_world = std::make_shared<World>();
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
	void World::loadObject(const AssetData& assetData) const
	{
		if (assetData.m_assetType == meta::Type(meta::TypeIDs<GStaticMesh>::ID))
		{
			std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
			std::shared_ptr<TransformComponent> transformComponent = std::make_shared<TransformComponent>();
			transformComponent->SetRotation(math::rotationQuat(math::double3(math::radians(60.0), math::radians(135.0), math::radians(40.0))));
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
			//load model
			GStaticMesh gStaticMesh = meta::Type::DeserializeJson<GStaticMesh>(nlohmann::json::parse(fileContent));
			delete[] fileContent;
			std::shared_ptr<StaticMeshComponent> staticMeshComponent = std::make_shared<StaticMeshComponent>();
			staticMeshComponent->setGStaticMesh(gStaticMesh);
		}
	}
}