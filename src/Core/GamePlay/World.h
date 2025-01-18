#pragma once

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
	private:

		std::shared_ptr<Level> m_currentLevel;
	};
}