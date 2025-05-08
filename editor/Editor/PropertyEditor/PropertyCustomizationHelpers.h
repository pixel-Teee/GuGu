#pragma once

#include <vector>
#include <Core/Reflection/Type.h>

namespace GuGu {
	class Widget;
	struct AssetData;

	using OnAssetSelected = std::function<void(const AssetData&)>;

	namespace PropertyCustomizationHelpers
	{
		std::shared_ptr<Widget> makeAssetPickerWithMenu(const AssetData& initialObject, const std::vector<meta::Type>& allowClasses, OnAssetSelected onSet);
	}
}