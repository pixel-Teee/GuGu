#pragma once

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

		AssetPickerConfig()
		: m_initialAssetViewType(AssetViewType::Tile)
		{}
	};
	class Widget;
	std::shared_ptr<Widget> createAssetPicker(const AssetPickerConfig& assetPickerConfig);

}