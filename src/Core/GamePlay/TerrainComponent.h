#pragma once

#include "Component.h"
#include <nvrhi.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class TerrainComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		TerrainComponent();

		virtual ~TerrainComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual void PostLoad() override;

		void createTileData();

		std::shared_ptr<GTexture> getHeightTexture() const;
		std::shared_ptr<GTexture> getTerrainTexture1() const;
		std::shared_ptr<GTexture> getTerrainTexture2() const;
		std::shared_ptr<GTexture> getTerrainTexture3() const;
		std::shared_ptr<GTexture> getTerrainTexture4() const;
		std::shared_ptr<GTexture> getBlendTexture() const;

		//terrainTexture
		std::shared_ptr<AssetData> m_heightTexture;
		std::shared_ptr<AssetData> m_terrainTexture1;
		std::shared_ptr<AssetData> m_terrainTexture2;
		std::shared_ptr<AssetData> m_terrainTexture3;
		std::shared_ptr<AssetData> m_terrainTexture4;
		//blend texture
		std::shared_ptr<AssetData> m_blendTexture;
		
		//terrain info
		uint32_t m_terrainRows;
		uint32_t m_terrainCols;

		//one block
		uint32_t m_rows;
		uint32_t m_cols;
		uint32_t m_tileSize;

		//one tile data(一个块的顶点数据和索引数据)
		std::vector<math::float3> m_vertexData;
		std::vector<uint32_t> m_indexData;
		nvrhi::BufferHandle m_vertexBufferHandle;
		nvrhi::BufferHandle m_indexBufferHandle;
		nvrhi::BufferHandle m_terrainConstantBuffer;
		std::vector<nvrhi::BufferHandle> m_terrainPropertiesConstantBuffer;
	};
}