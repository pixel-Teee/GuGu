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
		if (m_allowedClasses.size() == 1 && m_allowedClasses[0] == typeof(meta::Object))
		{
			assetPickerConfig.m_filter.m_classNames.clear();
		}
		else
		{
			for (int32_t i = 0; i < m_allowedClasses.size(); ++i)
			{
				assetPickerConfig.m_filter.m_classNames.push_back(m_allowedClasses[i].GetName());
			}
		}

		menuContent = WIDGET_NEW(BoxWidget)
		.WidthOverride(300.0f)
		.HeightOverride(300.0f)
		.Content
		(
			createAssetPicker(assetPickerConfig)//asset viewer
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = menuContent;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());
	}

}