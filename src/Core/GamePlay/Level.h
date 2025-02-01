#pragma once

#include <Core/Reflection/Object.h>
#include <Core/Reflection/Array.h>
#include "GameObject.h"

namespace GuGu {
	class Level : public meta::Object {
	public:
		Level();

		virtual ~Level();

		virtual void Update(float fElapsedTimeSeconds);

		virtual void PostLoad() override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		virtual void OnSerialize(nlohmann::json& output) const override;

		virtual void OnDeserialize(const nlohmann::json& input) override;

		const Array<std::shared_ptr<GameObject>>& getGameObjects() const;

		Array<std::shared_ptr<GameObject>>& getGameObjects();

		void setGameObjects(const Array<std::shared_ptr<GameObject>>& gameObjects);

		void addGameObject(std::shared_ptr<GameObject> inGameObject);
	private:
		Array<std::shared_ptr<GameObject>> m_objects; //all game objects
	};
}