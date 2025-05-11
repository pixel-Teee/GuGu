#include <pch.h>

#include "TerrainComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>

namespace GuGu {

	TerrainComponent::TerrainComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		std::shared_ptr<AssetData> whiteTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_heightTexture = whiteTexture;
		m_terrainTexture1 = whiteTexture;
		m_terrainTexture2 = whiteTexture;
		m_terrainTexture3 = whiteTexture;
		m_terrainTexture4 = whiteTexture;
		m_blendTexture = whiteTexture;
		m_terrainCols = 10;
		m_terrainRows = 10;
		m_rows = 20;
		m_cols = 20;
		m_tileSize = 2;
		createTileData();
	}

	TerrainComponent::~TerrainComponent()
	{

	}

	meta::Object* TerrainComponent::Clone(void) const
	{
		TerrainComponent* terrainComponent = new TerrainComponent();
		terrainComponent->m_heightTexture   = m_heightTexture;
		terrainComponent->m_terrainTexture1 = m_terrainTexture1;
		terrainComponent->m_terrainTexture2 = m_terrainTexture2;
		terrainComponent->m_terrainTexture3 = m_terrainTexture3;
		terrainComponent->m_terrainTexture4 = m_terrainTexture4;
		terrainComponent->m_blendTexture    = m_blendTexture;
		terrainComponent->m_terrainCols		= m_terrainCols;
		terrainComponent->m_terrainRows		= m_terrainRows;
		terrainComponent->m_rows			= m_rows;
		terrainComponent->m_cols			= m_cols;
		terrainComponent->m_tileSize		= m_tileSize;

		return terrainComponent;
	}

	void TerrainComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type TerrainComponent::GetType() const
	{
		return typeof(TerrainComponent);
	}

	void TerrainComponent::PostLoad()
	{
		//createTileData();
	}

	void TerrainComponent::createTileData()
	{
		int32_t rows = m_rows + 1;
		int32_t cols = m_cols + 1;
		uint32_t faceCount = rows * cols * 2;

		float halfWidth = 0.5f * m_tileSize * cols;
		float halfDepth = 0.5f * m_tileSize * rows;

		float dx = m_tileSize;
		float dz = m_tileSize;

		for (uint32_t i = 0; i < rows; ++i)
		{
			float z = -halfDepth + i * dz;
			for (uint32_t j = 0; j < cols; ++j)
			{
				float x = -halfWidth + j * dx;
				m_vertexData.push_back(math::float3(x, 0, z));//no y
			}
		}

		m_indexData.reserve(faceCount * 3);

		uint32_t k = 0;
		for (uint32_t i = 0; i < rows - 1; ++i)
		{
			for (uint32_t j = 0; j < cols - 1; ++j)
			{			
				m_indexData.push_back((i + 1) * cols + j);
				m_indexData.push_back(i * cols + j + 1);
				m_indexData.push_back(i * cols + j);
	
				m_indexData.push_back((i + 1) * cols + j + 1);
				m_indexData.push_back(i * cols + j + 1);
				m_indexData.push_back((i + 1) * cols + j);
			}
		}
	}

	std::shared_ptr<GTexture> TerrainComponent::getHeightTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_heightTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture1() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture1->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture2() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture2->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture3() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture3->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getTerrainTexture4() const
	{
		return std::static_pointer_cast<GTexture>(m_terrainTexture4->m_loadedResource);
	}

	std::shared_ptr<GTexture> TerrainComponent::getBlendTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_blendTexture->m_loadedResource);
	}

}