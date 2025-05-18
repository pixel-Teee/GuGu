#include <pch.h>

#include "WaterComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>

namespace GuGu {

	WaterComponent::WaterComponent()
	{
		GuGuUtf8Str noFileExtensionsFileName = "white";
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		std::shared_ptr<AssetData> whiteTexture = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GTexture)));

		m_dirTexture = whiteTexture;
		m_mixedColorTexture = whiteTexture;
		m_colorTexture = whiteTexture;
		m_waterCols = 10;
		m_waterRows = 10;
		m_rows = 20;
		m_cols = 20;
		m_tileSize = 2;
		createTileData();
	}

	WaterComponent::~WaterComponent()
	{

	}

	meta::Object* WaterComponent::Clone(void) const
	{
		WaterComponent* waterComponent = new WaterComponent();
		waterComponent->m_dirTexture = m_dirTexture;
		waterComponent->m_mixedColorTexture = m_mixedColorTexture;
		waterComponent->m_colorTexture = m_colorTexture;
		waterComponent->m_waterCols = m_waterCols;
		waterComponent->m_waterRows = m_waterRows;
		waterComponent->m_rows = m_rows;
		waterComponent->m_cols = m_cols;
		waterComponent->m_tileSize = m_tileSize;

		return waterComponent;
	}

	void WaterComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type WaterComponent::GetType() const
	{
		return typeof(WaterComponent);
	}

	void WaterComponent::PostLoad()
	{

	}

	void WaterComponent::createTileData()
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

	std::shared_ptr<GTexture> WaterComponent::getDirTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_dirTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> WaterComponent::getMixedColorTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_mixedColorTexture->m_loadedResource);
	}

	std::shared_ptr<GTexture> WaterComponent::getColorTexture() const
	{
		return std::static_pointer_cast<GTexture>(m_colorTexture->m_loadedResource);
	}

}