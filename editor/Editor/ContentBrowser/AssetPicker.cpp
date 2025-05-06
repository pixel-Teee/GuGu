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
		std::shared_ptr<VerticalBox> verticalBox = WIDGET_NEW(VerticalBox);

		verticalBox->addSlot()
		.StretchHeight(1.0f)
		(
			WIDGET_ASSIGN_NEW(AssetView, m_assetViewPtr)
		);
	}

}