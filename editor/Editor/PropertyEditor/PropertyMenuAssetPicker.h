#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/AssetManager/AssetData.h>
#include "PropertyCustomizationHelpers.h"

namespace GuGu {
	class PropertyMenuAssetPicker : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyMenuAssetPicker>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;

			ARGUMENT_VALUE(AssetData, initialObject)
			ARGUMENT_VALUE(std::vector<meta::Type>, allowedClasses)

			UI_EVENT(OnAssetSelected, onAssetSelected)
		};
		PropertyMenuAssetPicker();

		virtual ~PropertyMenuAssetPicker();

		void init(const BuilderArguments& arguments);

	private:
		AssetData m_currentObject;

		std::vector<meta::Type> m_allowedClasses;
	};
}