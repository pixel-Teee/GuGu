#include <pch.h>

#include "ContentBrowser.h"
#include "PathView.h"
#include "AssetView.h"

#include <Core/UI/ComplexGradient.h>
#include <Core/UI/Splitter.h>
#include <Core/UI/Button.h>

#include <Editor/StyleSet/EditorStyleSet.h>

namespace GuGu {
	void ContentBrowser::init(const BuilderArguments& arguments)
	{
		std::vector<math::float4> blueGradientBackground;
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel1"));
		blueGradientBackground.push_back(EditorStyleSet::getStyleSet()->getColor("lightBlueLevel2"));

		m_childWidget = std::make_shared<SingleChildSlot>();
		m_childWidget->m_parentWidget = shared_from_this();

		m_childWidget->m_childWidget = WIDGET_NEW(Splitter)
		+ Splitter::Slot()
		.value(0.13f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(PathView, m_pathView)
				.onPathSelected(this, &ContentBrowser::pathSelected)
			)
		)
		+ Splitter::Slot()
		.value(0.84f)
		.sizeRule(Splitter::SizeRule::FractionOfParent)
		(
			WIDGET_NEW(ComplexGradient)
			.GradientColors(blueGradientBackground)
			.cornerRadius(math::float4(5.0f, 5.0f, 5.0f, 5.0f))
			.Content
			(
				WIDGET_ASSIGN_NEW(AssetView, m_assetView)
				.onGetAssetContextMenu(this, &ContentBrowser::onGetAssetContextMenu)
			)
		);

		//实际路径
		const GuGuUtf8Str defaultPath = "content";

		//将content实际路径转换为虚拟路径
		std::vector<GuGuUtf8Str> selectedPaths;
		selectedPaths.push_back(defaultPath);
		m_pathView->setSelectedPaths(selectedPaths);
		m_assetView->setSourcesData(defaultPath);
	}
	ContentBrowser::~ContentBrowser()
	{
	}
	void ContentBrowser::pathSelected(const GuGuUtf8Str& folderPath)
	{
		m_assetView->setSourcesData(folderPath);
	}
	std::shared_ptr<Widget> ContentBrowser::onGetAssetContextMenu()
	{
		GuGuUtf8Str sourcesData = m_assetView->getSourcesData();//当前所处于的文件夹

		std::shared_ptr<Button> importModelButton;
		std::shared_ptr<VerticalBox> menuContent;
		WIDGET_NEW(VerticalBox)
		+ VerticalBox::Slot()
		.FixedHeight()
		(
			WIDGET_ASSIGN_NEW(Button, importModelButton)
			.buttonSyle(EditorStyleSet::getStyleSet()->getStyle<ButtonStyle>(u8"normalBlueButton"))
			.Content
			(
				WIDGET_NEW(TextBlockWidget)
				.text(u8"Import Model")
				.textColor(math::float4(0.18f, 0.16f, 0.12f, 1.0f))
			)
		);
		return importModelButton;
	}
}