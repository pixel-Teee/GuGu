#include <pch.h>

#include "PropertyCustomizationHelpers.h"
#include "PropertyMenuAssetPicker.h"
#include "PropertyMenuGameObjectPicker.h"

namespace GuGu {

	namespace PropertyCustomizationHelpers
	{
		std::shared_ptr<Widget> makeAssetPickerWithMenu(const AssetData& initialObject, const std::vector<meta::Type>& allowClasses, OnAssetSelected onSet)
		{
			return WIDGET_NEW(PropertyMenuAssetPicker)
				   .initialObject(initialObject)
				   .allowedClasses(allowClasses)
				   .onAssetSelected(onSet);
		}

		std::shared_ptr<GuGu::Widget> makeGameObjectPickerWithMenu(const GameObjectLevelRef& inGameObjectLevelRef, OnGameObjectSelected onSet)
		{
			return WIDGET_NEW(PropertyMenuGameObjectPicker)
				   .onGameObjectSelected(onSet);
		}

	}
}