#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/AssetManager/ARFilter.h>
#include "ContentBrowserSingleton.h"

namespace GuGu {
	class AssetView;
	class AssetPicker : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<AssetPicker>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;

			ARGUMENT_VALUE(AssetPickerConfig, assetPickerConfig)
		};
		AssetPicker();

		virtual ~AssetPicker();

		void init(const BuilderArguments& arguments);

		void handleAssetSelectionChanged(const AssetData& inAssetData, SelectInfo::Type inSelectInfo);
	private:
		std::shared_ptr<AssetView> m_assetViewPtr;

		ARFilter m_currentBackendFilter;

		OnAssetSelected m_onAssetSelected;
	};
}