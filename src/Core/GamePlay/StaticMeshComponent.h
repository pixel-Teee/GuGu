#pragma once

#include "Component.h"

#include <Core/Model/StaticMesh.h>

namespace GuGu {
	class StaticMeshComponent : public Component
	{
	public:
		StaticMeshComponent();

		virtual ~StaticMeshComponent();

		void Update(float fElapsedTimeSeconds) override;

		void setGStaticMesh(const GStaticMesh& gStaticMesh);
	private:
		std::shared_ptr<GStaticMesh> m_staticMesh;
	};
}