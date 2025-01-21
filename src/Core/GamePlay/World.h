#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct AssetData;
	class Level;
	class World
	{
	public:
		World();

		virtual ~World();

		static std::shared_ptr<World>& getWorld();

		std::shared_ptr<Level> getCurrentLevel();

		const std::shared_ptr<Level> getCurrentLevel() const;

		void loadObject(const AssetData& assetData) const;

		void update(float fElapsedTimeSeconds);

		void setWorldToViewMatrix(const math::affine3& matrix);

		math::affine3 getWorldToViewMatrix() const;

		void setCamPos(const math::float3& camPos);

		math::float3 getCamPos() const;

		void setFov(float fov);

		float getFov() const;
	private:

		std::shared_ptr<Level> m_currentLevel;

		//editor camera matrix
		math::affine3 m_editorCameraMatrix;

		math::float3 m_camPos;

		float m_fov;
	};
}