#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct AssetData;
	class Level;
	class ViewportClient;
	class Demo;
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

		void setViewportClient(std::shared_ptr<ViewportClient> viewportClient);

		std::weak_ptr<ViewportClient> getViewportClient() const;

		void renderLevel(Demo* demoPass);
	private:
		std::shared_ptr<ViewportClient> m_viewportClient;

		std::shared_ptr<Level> m_currentLevel;
	};
}