#pragma once

#include <Core/Math/MyMath.h>

#include <functional>

namespace GuGu {
	struct AssetData;
	class Level;
	class ViewportClient;
	class Demo;
	class World
	{
	public:
		enum WorldState
		{
			Runtime,
			Editor
		};
		World();

		virtual ~World();

		static std::shared_ptr<World>& getWorld();

		std::shared_ptr<Level> getCurrentLevel();

		const std::shared_ptr<Level> getCurrentLevel() const;

		void loadObject(AssetData& assetData) const;

		void loadLevel(AssetData& assetData);

		void setLevel(std::shared_ptr<Level> inLevel);

		void update(float fElapsedTimeSeconds);

		void setViewportClient(std::shared_ptr<ViewportClient> viewportClient);

		std::weak_ptr<ViewportClient> getViewportClient() const;

		void renderLevel(Demo* demoPass);

		void switchState(WorldState state);

		std::vector<std::function<void()>> m_onLevelChanged;
	private:
		std::shared_ptr<ViewportClient> m_viewportClient;

		std::shared_ptr<Level> m_editorLevel;

		std::shared_ptr<Level> m_currentLevel;
	};
}