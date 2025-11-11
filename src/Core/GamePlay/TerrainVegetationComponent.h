#pragma once

#include "Component.h"
#include <Renderer/nvrhi.h>

namespace GuGu {
	class TerrainComponent;
	//terrain vegetation
	class TerrainVegetationComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		TerrainVegetationComponent();

		virtual ~TerrainVegetationComponent();

		virtual meta::Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual void PostLoad() override;

		float getYOffset(math::float3 worldTranslation) const;

		//const std::weak_ptr<GameObject>& getParentGameObject() const;

		std::weak_ptr<TerrainComponent>& getTerrainOwner();

		void setTerrainOwner(const std::weak_ptr<TerrainComponent>& inTerrain);

		std::weak_ptr<TerrainComponent> m_terrainOwner;
	};
}