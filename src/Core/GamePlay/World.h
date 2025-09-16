#pragma once

#include <Core/Math/MyMath.h>

#include <functional>

namespace GuGu {
	struct AssetData;
	class Level;
	class ViewportClient;
	class Demo;
	class GameObject;
	class World : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

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

		//load texture
		std::shared_ptr<AssetData> loadTexture(const GuGuUtf8Str& filePath);

		void setLevel(std::shared_ptr<Level> inLevel);

		void update(float fElapsedTimeSeconds);

		void setViewportClient(std::shared_ptr<ViewportClient> viewportClient);

		std::weak_ptr<ViewportClient> getViewportClient() const;

		void renderLevel(Demo* demoPass);

		void switchState(WorldState state);

		std::vector<std::function<void()>> m_onLevelChanged;

		std::vector<std::function<void(std::shared_ptr<GameObject>&)>> m_onObjectRemoved;

		std::vector<std::function<void(std::shared_ptr<GameObject>&)>> m_onObjectAdded;//add object

		std::vector<std::function<void(std::shared_ptr<GameObject>&, std::shared_ptr<GameObject>&)>> m_onObjectAttached;

		std::vector<std::function<void(std::shared_ptr<GameObject>&, std::shared_ptr<GameObject>&)>> m_onObjectDetched;

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual meta::Object* Clone(void) const override;

		virtual void PostLoad() override;
	private:
		std::shared_ptr<ViewportClient> m_viewportClient;

		std::shared_ptr<Level> m_editorLevel;

		std::shared_ptr<Level> m_currentLevel;
	};
}