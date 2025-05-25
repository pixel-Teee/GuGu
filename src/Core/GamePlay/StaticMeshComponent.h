#pragma once

#include "Component.h"

#include <Core/Model/StaticMesh.h>

namespace GuGu {
	struct AssetData;

	class StaticMeshComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		StaticMeshComponent();

		virtual ~StaticMeshComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		void setGStaticMesh(const std::shared_ptr<AssetData>& inAssetData);

		void setGStaticMesh(const GStaticMesh& gStaticMesh);

		void setGStaticMesh(const std::shared_ptr<GStaticMesh>& gStaticMesh);

		std::shared_ptr<GStaticMesh> getStaticMesh() const;

		std::shared_ptr<GStaticMesh> getStaticMesh();

		std::shared_ptr<AssetData> getStaticMeshAsset() const;

		void setStaticMeshAsset(const std::shared_ptr<AssetData> inAssetData);

		virtual meta::Type GetType() const override;
	private:
		//std::shared_ptr<GStaticMesh> m_staticMesh;
		std::shared_ptr<AssetData> m_staticMeshAsset;
	};
}