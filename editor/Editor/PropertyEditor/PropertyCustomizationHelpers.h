#pragma once

#include <vector>
#include <Core/Reflection/Type.h>

namespace GuGu {
	class Widget;
	struct AssetData;
	struct GameObjectLevelRef;
	class GameObject;

	using OnAssetSelected = std::function<void(const AssetData&)>;

	using OnGameObjectSelected = std::function<void(std::shared_ptr<GameObject>)>;

	namespace PropertyCustomizationHelpers
	{
		std::shared_ptr<Widget> makeAssetPickerWithMenu(const AssetData& initialObject, const std::vector<meta::Type>& allowClasses, OnAssetSelected onSet);

		std::shared_ptr<Widget> makeGameObjectPickerWithMenu(const GameObjectLevelRef& inGameObjectLevelRef, OnGameObjectSelected onSet);
	}
}