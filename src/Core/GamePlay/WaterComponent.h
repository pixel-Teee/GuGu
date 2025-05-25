#pragma once

#include "Component.h"
#include <nvrhi.h>

namespace GuGu {
	struct AssetData;
	class GTexture;
	class WaterComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		WaterComponent();

		virtual ~WaterComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual void PostLoad() override;

		void createTileData();

		std::shared_ptr<GTexture> getDirTexture() const;
		std::shared_ptr<GTexture> getMixedColorTexture() const;
		std::shared_ptr<GTexture> getColorTexture() const;

		//dir texture
		std::shared_ptr<AssetData> m_dirTexture;

		//mixedColor texture
		std::shared_ptr<AssetData> m_mixedColorTexture;

		//color texture
		std::shared_ptr<AssetData> m_colorTexture;
		
		//water info
		uint32_t m_waterRows;
		uint32_t m_waterCols;

		//one block
		uint32_t m_rows;
		uint32_t m_cols;
		uint32_t m_tileSize;

		//one tile data(一个块的顶点数据和索引数据)
		std::vector<math::float3> m_vertexData;
		std::vector<uint32_t> m_indexData;
		nvrhi::BufferHandle m_vertexBufferHandle;
		nvrhi::BufferHandle m_indexBufferHandle;
		nvrhi::BufferHandle m_waterConstantBuffer;
		std::vector<nvrhi::BufferHandle> m_waterPropertiesConstantBuffer;

	};
}