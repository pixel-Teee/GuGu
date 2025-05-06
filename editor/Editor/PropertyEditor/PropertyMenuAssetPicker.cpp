#include <pch.h>

#include "PropertyMenuAssetPicker.h"
#include <Core/UI/Box.h>
#include <Editor/ContentBrowser/ContentBrowserSingleton.h>

namespace GuGu {

	PropertyMenuAssetPicker::PropertyMenuAssetPicker()
	{

	}

	PropertyMenuAssetPicker::~PropertyMenuAssetPicker()
	{

	}

	void PropertyMenuAssetPicker::init(const BuilderArguments& arguments)
	{
		m_currentObject = arguments.minitialObject;
		m_allowedClasses = arguments.mallowedClasses;

		std::shared_ptr<Widget> menuContent;
		AssetPickerConfig assetPickerConfig;
		assetPickerConfig.m_initialAssetViewType = AssetViewType::List;//asset view 显示的方式

		menuContent = WIDGET_NEW(BoxWidget)
		.WidthOverride(300.0f)
		.HeightOverride(300.0f)
		.Content
		(
			createAssetPicker(assetPickerConfig)//asset viewer
		);
	}

}