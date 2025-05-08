#pragma once

#include <Core/AssetManager/ARFilter.h>
#include <Editor/PropertyEditor/PropertyCustomizationHelpers.h>

namespace GuGu {
	namespace AssetViewType
	{
		enum Type
		{
			List,
			Tile,
			Column,
			Max
		};
	}

	struct AssetPickerConfig
	{
		AssetViewType::Type m_initialAssetViewType;

		ARFilter m_filter;

		OnAssetSelected m_onAssetSelected;

		AssetPickerConfig()
		: m_initialAssetViewType(AssetViewType::Tile)
		{}
	};
	class Widget;
	std::shared_ptr<Widget> createAssetPicker(const AssetPickerConfig& assetPickerConfig);

}