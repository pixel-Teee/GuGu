#include <pch.h>
#include "AssetPicker.h"
#include <Core/UI/BoxPanel.h>
#include "AssetView.h"

namespace GuGu {

	AssetPicker::AssetPicker()
	{

	}

	AssetPicker::~AssetPicker()
	{

	}

	void AssetPicker::init(const BuilderArguments& arguments)
	{
		m_currentBackendFilter = arguments.massetPickerConfig.m_filter;//filter

		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

		verticalBox->addSlot()
		.StretchHeight(1.0f)
		(
			WIDGET_ASSIGN_NEW(AssetView, m_assetViewPtr)
			.initialBackendFilter(m_currentBackendFilter)
			.initialViewType(arguments.massetPickerConfig.m_initialAssetViewType)
		);

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();
		m_childWidget->m_childWidget = verticalBox;
		m_childWidget->m_childWidget->setParentWidget(shared_from_this());

		//实际路径
		const GuGuUtf8Str defaultPath = "content";

		//将content实际路径转换为虚拟路径
		std::vector<GuGuUtf8Str> selectedPaths;
		selectedPaths.push_back(defaultPath);
		m_assetViewPtr->setSourcesData(defaultPath);
		m_assetViewPtr->requestSlowFullListRefresh();
	}

}