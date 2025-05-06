#include <pch.h>
#include "ContentBrowserSingleton.h"
#include "AssetPicker.h"

namespace GuGu {

	std::shared_ptr<Widget> createAssetPicker(const AssetPickerConfig& assetPickerConfig)
	{
		return WIDGET_NEW(AssetPicker)
			   .assetPickerConfig(assetPickerConfig);
	}

}