#pragma once

#include <Core/GuGuUtf8Str.h>
#include <Core/Reflection/Type.h>

namespace GuGu {
	class GameObject;
	struct GameObjectLevelRef : public meta::Object
	{
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GuGuUtf8Str m_levelFilePath; //level file path
		GuGuUtf8Str m_levelFileName; //level file name
		GuGuUtf8Str m_gameObjectName; //ref game object

		virtual void Update(float fElapsedTimeSeconds) override;

		virtual void PostLoad() override;

		virtual meta::Type GetType() const override;

		virtual Object* Clone(void) const override;

		GameObjectLevelRef() {}

		GameObjectLevelRef(const GameObjectLevelRef& rhs);

		GameObjectLevelRef(const GuGuUtf8Str& levelFilePath, const GuGuUtf8Str& levelFileName, const GuGuUtf8Str& gameObjectName);

		GameObjectLevelRef& operator=(const GameObjectLevelRef& rhs);

		GuGuUtf8Str toString() const;

		std::shared_ptr<GameObject> getGameObject();
	};
}